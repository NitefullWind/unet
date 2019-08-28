#ifndef TINYSERVER_TCPCLIENT_H
#define TINYSERVER_TCPCLIENT_H

#include <memory>
#include <mutex>
#include <tinyserver/inetAddress.h>
#include <tinyserver/types.h>

namespace tinyserver
{

class Connector;
class EventLoop;
class TcpConnection;

class TcpClient
{
public:
	TcpClient(EventLoop *loop, const InetAddress& serverAddress);
	~TcpClient();

	void connect();
	void disconnect();
	void stop();

	EventLoop *eventLoop() const { return _loop; }
	std::shared_ptr<TcpConnection> tcpConnection() const;

	bool retry() const { return _retry; }
	// 设置是否断线重连
	void setRetry(bool retry) { _retry = retry; }

	void setConnectionCallback(const ConnectionCallback& cb) { _connectionCallback = cb; }
	void setDisconnectionCallback(const DisconnectionCallback& cb) { _disconnetionCallback = cb; }
	void setMessageCallback(const MessageCallback& cb) { _messageCallback = cb; }

private:
	EventLoop *_loop;
	std::shared_ptr<Connector> _connector;
	std::mutex _mutex;
	std::shared_ptr<TcpConnection> _tcpConnection;
	uint32_t _nextConnId;
	bool _retry;
	bool _connect;

	ConnectionCallback _connectionCallback;
	DisconnectionCallback _disconnetionCallback;
	MessageCallback _messageCallback;

	void onNewConnection(int sockfd);
	void removeConnection(const std::shared_ptr<TcpConnection>& conn);
};

} // namespace tinyserver

#endif // TINYSERVER_TCPCLIENT_H
