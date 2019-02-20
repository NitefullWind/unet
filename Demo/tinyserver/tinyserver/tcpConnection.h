#ifndef TINYSERVER_TCPCONNECTION_H
#define TINYSERVER_TCPCONNECTION_H

#include <tinyserver/buffer.h>
#include <tinyserver/inetAddress.h>
#include <tinyserver/types.h>

namespace tinyserver
{

class Channel;
class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	explicit TcpConnection(EventLoop *loop, int sockfd);
	~TcpConnection();

	size_t index() const { return _index; }
	void setIndex(size_t index) { _index = index; }

	EventLoop *ownerLoop() const { return _loop; }

	const InetAddress& localAddress() const { return _localAddress; }
	const InetAddress& peerAddress() const { return _peerAddress; }

	void setCloseCallback(const CloseCallback& cb) {
		_closeCallback = cb;
	}

	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;
	}

	void connectionEstablished();
	void connectionDestroyed();

	void send(Buffer *buffer);
	void send(const std::string& str);
	void send(const char *data, size_t len);
	void send(const void *data, size_t len);
	void shutdown();
private:
	EventLoop *_loop;
	std::unique_ptr<Channel> _channel;
	size_t _index;
	InetAddress _localAddress;
	InetAddress _peerAddress;

	Buffer _inputBuffer;
	Buffer _outputBuffer;

	CloseCallback _closeCallback;
	MessageCallback _messageCallback;

	void onClose();
	void onReading();
	void onWriting();

	void sendInLoop(const void *data, size_t len);
	void shutdownInLoop();
};

}

#endif // TINYSERVER_TCPCONNECTION_H
