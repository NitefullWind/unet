#ifndef TINYSERVER_TCPSERVER_H
#define TINYSERVER_TCPSERVER_H

#include <tinyserver/inetAddress.h>

namespace tinyserver
{

class EventLoop;

class TcpServer
{
public:
	explicit TcpServer(EventLoop *loop, const InetAddress& inetAddress);
	~TcpServer();

	void start();

private:
	EventLoop *_loop;
	InetAddress _inetAddress;
	int _sockfd;
};

}

#endif	// TINYSERVER_TCPSERVER_H
