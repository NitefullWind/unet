#include <tinyserver/tcp/Connector.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <assert.h>
#include <errno.h>

using namespace tinyserver;

Connector::Connector(EventLoop *loop, const InetAddress& inetAddress):
	_loop(loop),
	_connect(false),
	_state(kDisconnected),
	_serverAddress(inetAddress)
{
}

Connector::~Connector()
{
}

void Connector::start()
{
	_connect = true;
	_loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::restart()
{
	_connect = true;
	_loop->runInLoop(std::bind(&Connector::restartInLoop, this));
}

void Connector::stop()
{
	_connect = false;
	_loop->runInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::startInLoop()
{
	_loop->assertInLoopThread();
	assert(_state == kDisconnected);
	if(_connect) { // 没有被调用stop停止连接
		connect();
	};
}

void Connector::restartInLoop()
{
	_loop->assertInLoopThread();
	setState(kDisconnected);
	startInLoop();
}

void Connector::stopInLoop()
{
	_loop->assertInLoopThread();
	if(_state == kConnecting) {
		setState(kDisconnected);
		int sockfd = removeAndResetChannel();
		retry(sockfd);
	}
}

void Connector::connect()
{
	int sockfd = sockets::CreateNonblockingSocket();
	int ret = sockets::Connect(sockfd, _serverAddress.sockAddrInet());
	int savedErrno = (ret == 0) ? 0 : errno;
	switch (savedErrno)
	{
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
		TLOG_ERROR("Connect error in Connector::startInLoop " << savedErrno);
		sockets::Close(sockfd);
		break;

	default:
		TLOG_ERROR("Unexpected error in Connector::startInLoop " << savedErrno);
		sockets::Close(sockfd);
		// connectErrorCallback_();
		break;
	}
}

void Connector::connecting(int sockfd)
{
	setState(kConnecting);
	assert(!_channel);
	_channel.reset(new Channel(_loop, sockfd));
	_channel->setWriteCallback(std::bind(&Connector::onWrite, this));
	_channel->setErrorCallback(std::bind(&Connector::onError, this));
	_channel->enableWriting();
}

void Connector::retry(int sockfd)
{
	sockets::Close(sockfd);
	setState(kDisconnected);
	if(_connect) { // 没有被调用stop停止连接
		TLOG_INFO(__FUNCTION__ << " Retry connecting to " << _serverAddress.toHostPort());
		//! FIXME: should be call runAfter()
		_loop->queueInLoop(std::bind(&Connector::startInLoop, this));
	}
}

void Connector::onWrite()
{
	TLOG_DEBUG(__FUNCTION__ << " state: " << _state);
	if(_state == kDisconnecting) {
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if(err) {
			TLOG_WARN(__FUNCTION__ << " SO_ERROR = " << err << " " << strerror(err));
			retry(sockfd);
		} else if(sockets::IsSelfConnect(sockfd)) {
			TLOG_WARN(__FUNCTION__ << " Self Connect");
			retry(sockfd);
		} else {
			setState(kConnected);
			if(_connect) {
				_newConnectionCallback(sockfd);
			} else { // 已调用stop停止连接
				sockets::Close(sockfd);
			}
		}
	} else {
		assert(_state == kDisconnected);
	}
}

int Connector::removeAndResetChannel()
{
	_channel->disableAll();
	_loop->removeChannel(_channel.get());
	int sockfd = _channel->fd();
	_loop->queueInLoop([this](){
		this->_channel.reset();
	});
	return sockfd;
}