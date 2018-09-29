#include <muduo/net/muduo.h>
#include <muduo/net/tcpServer.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/inetAddress.h>
#include <muduo/net/buffer.h>

#include <iostream>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr& conn)
{
	if(conn->connected()) {
		std::cout << "onConnection(): tid = " << std::this_thread::get_id()
			<< " new connection " << conn->name() 
			<< " from " << conn->peerAddress().toHostPort() << std::endl;
	} else {
		std::cout << "onConnection(): connetion " << conn->name() << " is down\n";
	}
}

void onMessage(const TcpConnectionPtr& conn, Buffer *buf, Timestamp receiveTime)
{
	std::cout << "onMessage(): tid = " << std::this_thread::get_id() 
			<< " receive " << buf->readableBytes() 
			<< " bytes from connection " << conn->peerAddress().toHostPort() 
			<< " at " << receiveTime.toFormattedString() << std::endl;
	conn->send(buf->retrieveAsString());
}

int main(int argc, char *argv[])
{
	std::cout << "main(): pid = " << getpid() << std::endl;

	InetAddress listenAddr(9981);
	EventLoop loop;

	TcpServer server(&loop, listenAddr);
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	if(argc > 1) {
		server.setThreadNum(atoi(argv[1]));
	}
	server.start();

	loop.loop();
}
