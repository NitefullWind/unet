#ifndef MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

class EchoServer
{
public:
	EchoServer(EventLoop* loop, const InetAddress &listenAddr);

	void start();	// calls server_.start();

private:
	void onConnection(const TcpConnectionPtr &conn);
	void onMessage(const TcpConnectionPtr &conn, Buffer* buf, Timestamp time);

	TcpServer server_;
};

#endif // MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
