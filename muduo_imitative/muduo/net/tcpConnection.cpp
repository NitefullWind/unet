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
}

void TcpConnection::handleClose()
{
	_loop->assertInLoopThread();
	LOG_TRACE("TcpConnetion::handleClose state = " << _state);
	assert(_state == kConnected);
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
	assert(_state == kConnected);
	setState(kDisconnected);
	_channel->disableAll();
	_connectionCallback(shared_from_this());

	_loop->removeChannel(_channel.get());
}
