#include <muduo/net/tcpConnection.h>
#include <muduo/net/channel.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/socket.h>
#include <muduo/base/logger.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
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
	_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this));
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

void TcpConnection::handleRead()
{
	char buf[65536];
	ssize_t n = ::read(_channel->fd(), buf, sizeof(buf));
	_messageCallback(shared_from_this(), buf, n);
}
