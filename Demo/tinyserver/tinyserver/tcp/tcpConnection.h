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
	explicit TcpConnection(EventLoop *loop, const std::string id, int sockfd);
	~TcpConnection();

	std::string id() const { return _id; }

	EventLoop *ownerLoop() const { return _loop; }

	const InetAddress& localAddress() const { return _localAddress; }
	const InetAddress& peerAddress() const { return _peerAddress; }

	void setCloseCallback(const CloseCallback& cb) {
		_closeCallback = cb;
	}

	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;
	}

	void setConnectionCallback(const ConnectionCallback& cb) {
		_connectionCallback = cb;
	}

	void setDisonnectionCallback(const DisconnectionCallback& cb) {
		_disconnectionCallback = cb;
	}

	void connectionEstablished();
	void connectionDestroyed();

	void send(const std::string& str);
	void send(const char *data, size_t len);
	void send(Buffer *buffer);
	void send(const void *data, size_t len);
	void shutdown();
private:
	enum StateEnum { kConnecting, kConnected, kDisconnecting, kDisconnected };

	EventLoop *_loop;
	StateEnum _state;
	std::unique_ptr<Channel> _channel;
	const std::string _id;
	InetAddress _localAddress;
	InetAddress _peerAddress;

	Buffer _inputBuffer;
	Buffer _outputBuffer;

	CloseCallback _closeCallback;
	MessageCallback _messageCallback;
	ConnectionCallback _connectionCallback;
	DisconnectionCallback _disconnectionCallback;

	void onClose();
	void onReading();
	void onWriting();
	void onError();

	void sendInLoop(const Buffer& buffer);
	void sendInLoop(const void *data, size_t len);
	void shutdownInLoop();

	void setState(StateEnum state) { this->_state = state; }
};

}

#endif // TINYSERVER_TCPCONNECTION_H
