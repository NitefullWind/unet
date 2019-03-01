#include <tinyserver/tcpConnection.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <functional>
#include <assert.h>

using namespace tinyserver;

TcpConnection::TcpConnection(EventLoop *loop, int sockfd) :
	_loop(loop),
	_state(kConnecting),
	_channel(new Channel(loop, sockfd)),
	_index(-1),
	_localAddress(sockets::GetLocalAddr(sockfd)),
	_peerAddress(sockets::GetPeerAddr(sockfd))
{
	// _channel->enableWriting();
	_channel->setCloseCallback(std::bind(&TcpConnection::onClose, this));
	_channel->setReadCallback(std::bind(&TcpConnection::onReading, this));
	_channel->setWriteCallback(std::bind(&TcpConnection::onWriting, this));
}

TcpConnection::~TcpConnection()
{
	LOG_TRACE(__FUNCTION__ << " index: " << _index);
	sockets::Close(_channel->fd());
}

void TcpConnection::connectionEstablished()
{
	_loop->assertInLoopThread();
	assert(_state == kConnecting);
	setState(kConnected);

	_channel->enableReading();	// this must be called in this loop thread
}

void TcpConnection::connectionDestroyed()
{
	_loop->assertInLoopThread();
	if(_state == kConnected) {
		setState(kDisconnected);
		_channel->disableAll();
	}
	_loop->removeChannel(_channel.get());
}

void TcpConnection::send(Buffer *buffer)
{
	std::string str = buffer->readAll();
	send(str);
}

void TcpConnection::send(const std::string& str)
{
	send(str.data(), str.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	send((void*)data, len);
}

void TcpConnection::send(const void *data, size_t len)
{
	if(_state == kConnected) {
		if(_loop->isInLoopThread()) {
			sendInLoop(data, len);
		} else {
			_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, data, len));
		}
	}
}

void TcpConnection::onClose()
{
	_loop->assertInLoopThread();
	LOG_TRACE("close connection: " << _channel->fd());
	assert(_state == kConnected || _state == kDisconnecting);
	_channel->disableAll();
	if(_closeCallback) {
		_closeCallback(shared_from_this());		// it call TcpConnection::connectionDestroyed in TcpServer
	}
}

void TcpConnection::onReading()
{
	LOG_TRACE(__FUNCTION__);
	size_t n = _inputBuffer.readFd(_channel->fd());
	if(n > 0) {
		if(_messageCallback) {
			_messageCallback(shared_from_this(), &_inputBuffer);
		}
	} else if (n == 0) {
		onClose();
	} else {
		onClose();
		LOG_ERROR(__FUNCTION__ << " error");
	}
}

void TcpConnection::onWriting()
{
	LOG_TRACE(__FUNCTION__);
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
	if(_state == kConnected) {
		_loop->assertInLoopThread();
		sockets::Writen(_channel->fd(), data, len);
	}
}

void TcpConnection::shutdown()
{
	if(_state == kConnected) {
		setState(kDisconnected);

		if(_loop->isInLoopThread()) {
			shutdownInLoop();
		} else {
			_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
		}
	}
}

void TcpConnection::shutdownInLoop()
{
	_loop->assertInLoopThread();
	sockets::ShutdownWrite(_channel->fd());
}
