#ifndef TINYSERVER_TCPSERVER_H
#define TINYSERVER_TCPSERVER_H

#include <tinyserver/inetAddress.h>

#include <functional>

namespace tinyserver
{

class EventLoop;

typedef std::function<void(const InetAddress& address)> ConnectionCallback;

class TcpServer
{
public:
	explicit TcpServer(EventLoop *loop, const InetAddress& inetAddress);
	~TcpServer();

	void start();

	void setConnectionCallback(const ConnectionCallback& cb);

private:
	EventLoop *_loop;
	InetAddress _inetAddress;
	int _sockfd;
	ConnectionCallback _connectCallback;
};

}

#endif	// TINYSERVER_TCPSERVER_H
