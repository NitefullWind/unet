#include <muduo/net/tcpConnection.h>
#include <muduo/net/channel.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/socket.h>
#include <muduo/net/socketsOps.h>
#include <muduo/net/buffer.h>
#include <muduo/base/logger.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

TcpConnection::TcpConnection(EventLoop *loop, const std::string& nameArg, int sockfd,
							const InetAddress& localAddr, const InetAddress& peerAddr):
	_loop(loop),
	_name(nameArg),
	_state(kConnecting),
	_socket(new Socket(sockfd)),
	_channel(new Channel(loop, sockfd)),
	_localAddr(localAddr),
	_peerAddr(peerAddr)
{
	LOG_DEBUG("TcpConnection::ctor[" << _name << "] at " << this << " fd = " << sockfd);
	_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
}

TcpConnection::~TcpConnection()
{
	LOG_DEBUG("TcpConnection::dtor[" << _name << "] at " << this << " fd = " << _channel->fd());
}

void TcpConnection::send(const std::string& message)
{
	if(_state == kConnected) {
		if(_loop->isInLoopThread()) {
			sendInLoop(message);
		} else {
			_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, shared_from_this(), message));
		}
	}
}

void TcpConnection::sendInLoop(const std::string& message)
{
	_loop->assertInLoopThread();
	ssize_t nwrote = 0;
	// if no thing in output queue, try writing directly
	if(!_channel->isWriting() && _outputBuffer.readableBytes() == 0) {
		nwrote = ::write(_channel->fd(), message.data(), message.size());
		if(nwrote >= 0) {
			if((size_t)(nwrote) < message.size()) {
				LOG_TRACE("I am going to write more data");
			}
		} else {
			nwrote = 0;
			if(errno != EWOULDBLOCK) {
				LOG_FATAL("TcpConnection::sendInLoop()");
			}
		}
	}

	assert(nwrote >= 0);
	if((size_t)(nwrote) < message.size()) {
		_outputBuffer.append(message.data()+nwrote, message.size()-nwrote);
		if(!_channel->isWriting()) {
			_channel->enableWriting();
		}
	}
}

void TcpConnection::shutdown()
{
	// FIXME: use compare and swap
	if(_state == kConnected) {
		setState(kDisconnecting);
		// FIXME: shared_from_this()
		_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
	}
}

void TcpConnection::shutdownInLoop()
{
	_loop->assertInLoopThread();
	if(!_channel->isWriting()) {
		// we are not writing
		_socket->shutdownWrite();
	}
}

void TcpConnection::connectEstablished()
{
	_loop->assertInLoopThread();
	assert(_state == kConnecting);
	setState(kConnected);
	_channel->enableReading();

	_connectionCallback(shared_from_this());
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	int savedError = 0;
	ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedError);
	if ( n > 0) {
		_messageCallback(shared_from_this(), &_inputBuffer, receiveTime);
	} else if (n == 0) {
		handleClose();
	} else {
		errno = savedError;
		LOG_ERROR("TcpConnection::handleRead");
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	_loop->assertInLoopThread();
	if(_channel->isWriting()) {
		ssize_t n = ::write(_channel->fd(), _outputBuffer.peek(), _outputBuffer.readableBytes());
		if(n > 0) {
			_outputBuffer.retrieve(n);
			if(_outputBuffer.readableBytes() == 0) {
				_channel->disableWriting();
				if(_state == kDisconnecting) {
					shutdownInLoop();
				}
			} else {
				LOG_TRACE("I am going to write more data");
			}
		} else {
			LOG_FATAL("TcpConnection::handleWrite");
		}
	} else {
		LOG_TRACE("Connection is down, no more writing");
	}
}

void TcpConnection::handleClose()
{
	_loop->assertInLoopThread();
	LOG_TRACE("TcpConnetion::handleClose state = " << _state);
	assert(_state == kConnected || _state == kDisconnecting);
	_channel->disableAll();
	_closeCallback(shared_from_this());
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(_channel->fd());
	LOG_ERROR("TcpConnection::handleError [" << _name 
			<< "] - SO_ERROR = " << err << " " << strerror(err));
}

void TcpConnection::connectDestroyed()
{
	_loop->assertInLoopThread();
	assert(_state == kConnected || _state == kDisconnecting);
	setState(kDisconnected);
	_channel->disableAll();
	_connectionCallback(shared_from_this());

	_loop->removeChannel(_channel.get());
}
