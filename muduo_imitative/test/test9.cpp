/*
 * test Connector
 */

#include <muduo/net/connector.h>
#include <muduo/net/eventLoop.h>

#include <iostream>

muduo::net::EventLoop *g_loop;

void connectCallback(int sockfd)
{
	std::cout << "connected.\n";
	g_loop->quit();
}

int main(int argc, char *argv[])
{
	muduo::net::EventLoop loop;
	g_loop = &loop;
	muduo::net::InetAddress addr("127.0.0.1", 9981);
	muduo::net::ConnectorPtr connector(new muduo::net::Connector(&loop, addr));
	connector->setNewConnectionCallback(connectCallback);
	connector->start();

	loop.loop();
}
