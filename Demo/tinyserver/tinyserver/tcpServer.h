#ifndef TINYSERVER_TCPSERVER_H
#define TINYSERVER_TCPSERVER_H

#include <tinyserver/inetAddress.h>

#include <functional>
#include <list>
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

private:
	EventLoop *_loop;
	InetAddress _inetAddress;
	std::unique_ptr<Channel> _channel;
	NewConnectionCallback _newConnectionCallback;
	std::list<std::unique_ptr<TcpConnection> > _connections;
};

}

#endif	// TINYSERVER_TCPSERVER_H
