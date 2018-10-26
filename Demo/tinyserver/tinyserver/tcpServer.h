#ifndef TINYSERVER_TCPSERVER_H
#define TINYSERVER_TCPSERVER_H

#include <tinyserver/inetAddress.h>
#include <tinyserver/types.h>

#include <map>

namespace tinyserver
{

class Channel;
class TcpConnection;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
public:
	explicit TcpServer(EventLoop *loop, const InetAddress& inetAddress);
	~TcpServer();

	void start();

	void setConnectionCallback(const ConnectionCallback& cb) {
		_newConnectionCallback = cb;
	}
	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;
	}

	void setIOThreadNum(size_t numThreads);

private:
	EventLoop *_loop;
	InetAddress _inetAddress;
	std::unique_ptr<Channel> _channel;
	uint64_t _connectionCounter;
	ConnectionCallback _newConnectionCallback;
	MessageCallback _messageCallback;
	std::map<size_t, TcpConnectionPtr> _connectionMap;
	std::shared_ptr<EventLoopThreadPool> _IOThreadPool;

	void onNewConnection();
	void onNewMessage();

	void removeConnection(const TcpConnectionPtr& tcpConnPtr);
};

}

#endif	// TINYSERVER_TCPSERVER_H
