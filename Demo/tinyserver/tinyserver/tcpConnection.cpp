#include <tinyserver/tcpConnection.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <functional>

using namespace tinyserver;

TcpConnection::TcpConnection(EventLoop *loop, int sockfd) :
	_channel(new Channel(loop, sockfd))
{
	_channel->enableReading();
	// _channel->enableWriting();
	_channel->setCloseCallback(std::bind(&TcpConnection::onClose, this));
	_channel->setReadCallback(std::bind(&TcpConnection::onReading, this));
	_channel->setWriteCallback(std::bind(&TcpConnection::onWriting, this));
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::onClose()
{
	LOG_TRACE("close connection: " << _channel->fd());
	_channel->disableAll();
	//! TODO call remove
	sockets::Close(_channel->fd());
}

void TcpConnection::onReading()
{
	char buf[1024];
	sockets::Read(_channel->fd(), buf, sizeof(buf));
	LOG_TRACE(__FUNCTION__ << ": " << buf);
}

void TcpConnection::onWriting()
{
	char buf[] = "welcome!"; 
	sockets::Write(_channel->fd(), buf, sizeof(buf));
	LOG_TRACE(__FUNCTION__);
}
