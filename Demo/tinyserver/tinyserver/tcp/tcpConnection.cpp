#include <tinyserver/tcp/tcpConnection.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <assert.h>
#include <functional>
#include <unistd.h>

using namespace tinyserver;

TcpConnection::TcpConnection(EventLoop *loop, const std::string id, int sockfd) :
	_loop(loop),
	_state(kConnecting),
	_channel(new Channel(loop, sockfd)),
	_id(id),
	_localAddress(sockets::GetLocalAddr(sockfd)),
	_peerAddress(sockets::GetPeerAddr(sockfd))
{
	// _channel->enableWriting();
	_channel->setCloseCallback(std::bind(&TcpConnection::onClose, this));
	_channel->setReadCallback(std::bind(&TcpConnection::onReading, this));
	_channel->setWriteCallback(std::bind(&TcpConnection::onWriting, this));
	_channel->setErrorCallback(std::bind(&TcpConnection::onError, this));
}

TcpConnection::~TcpConnection()
{
	TLOG_TRACE(__FUNCTION__ << " id: " << _id);
	sockets::Close(_channel->fd());
}

void TcpConnection::connectionEstablished()
{
	_loop->assertInLoopThread();
	assert(_state == kConnecting);
	setState(kConnected);

	_channel->enableReading();	// this must be called in this loop thread
	if(_connectionCallback) {
		_connectionCallback(shared_from_this());
	}
}

void TcpConnection::connectionDestroyed()
{
	_loop->assertInLoopThread();
	if(_state == kConnected) {
		setState(kDisconnected);
		_channel->disableAll();
	}
	_loop->removeChannel(_channel.get());
	if(_disconnectionCallback) {
		_disconnectionCallback(shared_from_this());
	}
}

void TcpConnection::send(const std::string& str)
{
	send(str.data(), str.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	send((void*)data, len);
}

void TcpConnection::send(Buffer *buffer)
{
	if(_state == kConnected) {
		if(_loop->isInLoopThread()) {
			sendInLoop(*buffer);
		} else {
			Buffer buf(*buffer);
			_loop->runInLoop([this, buf](){
				sendInLoop(buf);
			});
		}
	}
}

void TcpConnection::send(const void *data, size_t len)
{
	if(_state == kConnected) {
		if(_loop->isInLoopThread()) {
			sendInLoop(data, len);
		} else {
			// Fix: runInLoop中的函数，参数时必须按值传递，否则再另一个线程中取地址无效
			Buffer buf(len);
			buf.append(data, len);
			// _loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, buf));
			_loop->runInLoop([this, buf](){
				sendInLoop(buf);
			});
		}
	}
}

void TcpConnection::sendInLoop(const Buffer& buffer)
{
	sendInLoop(buffer.data(), buffer.readableBytes());
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
	_loop->assertInLoopThread();
	if(_state == kConnected) {
		ssize_t nwrote = 0;
		if(!_channel->isWriting() && _outputBuffer.readableBytes() == 0) {
			nwrote = ::write(_channel->fd(), data, len);
			if(nwrote < 0) {
				nwrote = 0;
				if(errno != EWOULDBLOCK) {
					TLOG_ERROR(__FUNCTION__ << " error");
				}
			}
		}
		TLOG_TRACE("Connection fd = " << _channel->fd() << " send data, size: " << nwrote);

		if((size_t)nwrote < len) {						// there are more data need to write
			TLOG_TRACE("sendInLoop there are more data need to write")
			_outputBuffer.append((const char *)data + nwrote, len - (size_t)nwrote);
			if(!_channel->isWriting()) {
				_channel->enableWriting();
			}
		}
	}
}

void TcpConnection::onClose()
{
	_loop->assertInLoopThread();
	TLOG_TRACE("close connection: " << _channel->fd());
	assert(_state == kConnected || _state == kDisconnecting);
	_channel->disableAll();
	if(_closeCallback) {
		_closeCallback(shared_from_this());		// it call TcpConnection::connectionDestroyed in TcpServer
	}
}

void TcpConnection::onReading()
{
	TLOG_TRACE(__FUNCTION__);
	int savedError = 0;
	ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedError);
	if(n > 0) {
		if(_messageCallback) {
			_messageCallback(shared_from_this(), &_inputBuffer);
		}
	} else if (n == 0) {
		onClose();
	} else {
		TLOG_ERROR(__FUNCTION__ << " error");
		onError();
	}
}

void TcpConnection::onWriting()
{
	TLOG_TRACE(__FUNCTION__);
	_loop->assertInLoopThread();
	if(_channel->isWriting()) {							// write
		ssize_t nwrote = sockets::Writen(_channel->fd(), _outputBuffer.peek(), _outputBuffer.readableBytes());
		if(nwrote < 0) {
			nwrote = 0;
			if(errno != EWOULDBLOCK) {
				TLOG_ERROR(__FUNCTION__ << " error");
			}
		} else {
			_outputBuffer.retrieve((size_t)nwrote);
		}
		
		if(_outputBuffer.readableBytes() == 0) {		// no more data to write
			_channel->disableWriting();					// disable writing
			if(_state == kDisconnecting) {				// need to shutdown the connection
				shutdownInLoop();
			}
		}
	} else {
		//!TODO: what's this?
		TLOG_TRACE("Connection fd = " << _channel->fd() << " is down, no more writing.");
	}
}

void TcpConnection::onError()
{
	int err = sockets::getSocketError(_channel->fd());
	TLOG_ERROR(__FUNCTION__ << " Id:" << _id << ". localAddress:"
	 << _localAddress.toHostPort() << " peerAddress:" << _peerAddress.toHostPort()
	 << ", errno:" << err << " errmsg:" << strerror(err));
}

void TcpConnection::shutdown()
{
	if(_state == kConnected) {
		setState(kDisconnecting);

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
