#include <tinyserver/tcpConnection.h>
#include <tinyserver/buffer.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <functional>

using namespace tinyserver;

TcpConnection::TcpConnection(EventLoop *loop, int sockfd) :
	_loop(loop),
	_channel(new Channel(loop, sockfd)),
	_index(-1)
{
	_channel->enableReading();
	// _channel->enableWriting();
	_channel->setCloseCallback(std::bind(&TcpConnection::onClose, this));
	_channel->setReadCallback(std::bind(&TcpConnection::onReading, this));
	_channel->setWriteCallback(std::bind(&TcpConnection::onWriting, this));
}

TcpConnection::~TcpConnection()
{
	LOG_TRACE(__FUNCTION__ << " index: " << _index);
}

void TcpConnection::onClose()
{
	LOG_TRACE("close connection: " << _channel->fd());
	_loop->removeChannel(_channel.get());
	//! FIXME: Should check the state of channel, cann't be writting.
	if(_closeCallback) {
		_closeCallback(_index);
	}
}

void TcpConnection::onReading()
{
	Buffer buffer;
	size_t n = buffer.readFd(_channel->fd());
	if(n > 0) {
		LOG_TRACE(__FUNCTION__ << ": " << buffer.readAll());
	} else if (n == 0) {
		onClose();
	} else {
		onClose();
		LOG_ERROR(__FUNCTION__ << " error");
	}
}

void TcpConnection::onWriting()
{
	char buf[] = "welcome!"; 
	sockets::Write(_channel->fd(), buf, sizeof(buf));
	LOG_TRACE(__FUNCTION__);
}
