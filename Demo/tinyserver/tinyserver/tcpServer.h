#ifndef TINYSERVER_TCPSERVER_H
#define TINYSERVER_TCPSERVER_H

#include <tinyserver/inetAddress.h>

#include <functional>
#include <map>
#include <memory>

namespace tinyserver
{

class Channel;
class TcpConnection;
class EventLoop;

typedef std::function<void(const InetAddress& address)> NewConnectionCallback;

class TcpServer
{
public:
	explicit TcpServer(EventLoop *loop, const InetAddress& inetAddress);
	~TcpServer();

	void start();

	void setConnectionCallback(const NewConnectionCallback& cb);

	void onNewConnection();

	void removeConnection(size_t index);

private:
	EventLoop *_loop;
	InetAddress _inetAddress;
	std::unique_ptr<Channel> _channel;
	NewConnectionCallback _newConnectionCallback;
	std::map<size_t, std::shared_ptr<TcpConnection> > _connectionMap;
};

}

#endif	// TINYSERVER_TCPSERVER_H
