#include <muduo/net/connector.h>
#include <muduo/net/channel.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/socketsOps.h>

#include <muduo/base/logger.h>

#include <errno.h>
#include <assert.h>
#include <string.h>	// strerror()

using namespace muduo;
using namespace muduo::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop *loop, const InetAddress& serverAddr):
	_loop(loop),
	_serverAddr(serverAddr),
	_connect(false),
	_state(kDisconnected),
	_retryDelayMs(kMaxRetryDelayMs)
{
}

Connector::~Connector()
{
	_loop->cancel(_timerId);
	assert(!_channel);
}

void Connector::start()
{
	_connect = true;
	_loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
	_loop->assertInLoopThread();
	assert(_state == kDisconnected);
	if(_connect) {
		connect();
	} else {
		LOG_DEBUG("do not connect");
	}
}

void Connector::connect()
{
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd, _serverAddr.socketAddrInet());
	int savedError = (ret == 0) ? 0 : errno;
	switch(savedError) {
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			break;
			
		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;

		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
			LOG_FATAL("connect error in Connector::startInLoop " << savedError);
			sockets::close(sockfd);
			break;

		default:
			LOG_FATAL("Unexpected error in Connector::startInLoop " << savedError);
			sockets::close(sockfd);
			break;
	}
}

void Connector::restart()
{
	_loop->assertInLoopThread();
	setState(kDisconnected);
	_retryDelayMs = kInitRetryDelayMs;
	_connect = true;
	startInLoop();
}

void Connector::stop()
{
	_connect = false;
	_loop->cancel(_timerId);
}

void Connector::connecting(int sockfd)
{
	setState(kConnecting);
	assert(!_channel);
	_channel.reset(new Channel(_loop, sockfd));
	_channel->setWriteCallback(std::bind(&Connector::handleWrite, this));
	_channel->setErrorCallback(std::bind(&Connector::handleError, this));

	_channel->enableWriting();
}

int Connector::removeAndResetChannel()
{
	_channel->disableAll();
	_loop->removeChannel(_channel.get());
	int sockfd = _channel->fd();
	_loop->queueInLoop(std::bind(&Connector::resetChannel, this));
	return sockfd;
}

void Connector::resetChannel()
{
	_channel.reset();
}

void Connector::handleWrite()
{
	LOG_TRACE("Connector::handleWrite " << _state);

	if(_state == kConnecting) {
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if(err) {
			LOG_WARN("Conector::handleWrite - SO_ERROR = " << err << " " << strerror(err));
			retry(sockfd);
		} else if(sockets::isSelfConnect(sockfd)) {
			LOG_WARN("Connector::handleWrite - Self connect");
			retry(sockfd);
		} else {
			setState(kConnected);
			if(_connect) {
				_newConnectionCallback(sockfd);
			} else {
				sockets::close(sockfd);
			}
		}
	} else {
		assert(_state == kDisconnected);
	}
}

void Connector::handleError()
{
	LOG_ERROR("Connector::handleError");
	assert(_state == kConnecting);

	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	LOG_TRACE("SO_ERROR = " << err << " " << strerror(err));
	retry(sockfd);
}

void Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(kDisconnected);
	if(_connect) {
		LOG_INFO("Conector::retry - Retry connecting to "
				<< _serverAddr.toHostPort() << " in " 
				<< _retryDelayMs << " milliseconds.");
		_timerId = _loop->runAfter(_retryDelayMs/1000.0, 
				std::bind(&Connector::startInLoop, this));
		_retryDelayMs = std::min(_retryDelayMs * 2, kMaxRetryDelayMs);
	} else {
		LOG_DEBUG("do not connect");
	}
}
