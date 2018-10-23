#ifndef TINYSERVER_TCPCONNECTION_H
#define TINYSERVER_TCPCONNECTION_H

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

	const InetAddress& localAddress() const { return _localAddress; }
	const InetAddress& peerAddress() const { return _peerAddress; }

	void setCloseCallback(const CloseCallback& cb) {
		_closeCallback = cb;
	}

	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;
	}

	void onClose();
	void onReading();
	void onWriting();
private:
	EventLoop *_loop;
	std::unique_ptr<Channel> _channel;
	size_t _index;
	InetAddress _localAddress;
	InetAddress _peerAddress;

	CloseCallback _closeCallback;
	MessageCallback _messageCallback;
};

}

#endif // TINYSERVER_TCPCONNECTION_H
