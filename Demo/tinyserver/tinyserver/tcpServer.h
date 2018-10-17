#ifndef TINYSERVER_TCPSERVER_H
#define TINYSERVER_TCPSERVER_H

#include <tinyserver/inetAddress.h>

#include <functional>
#include <memory>

namespace tinyserver
{

class Channel;
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
	std::unique_ptr<Channel> _channel;
	ConnectionCallback _connectCallback;
};

}

#endif	// TINYSERVER_TCPSERVER_H
