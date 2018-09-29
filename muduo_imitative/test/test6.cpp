#include <muduo/net/eventLoop.h>
#include <muduo/net/acceptor.h>
#include <muduo/net/inetAddress.h>
#include <muduo/net/socketsOps.h>
#include <iostream>
#include <unistd.h>

void newConnetion1(int sockfd, const muduo::net::InetAddress& peerAddr)
{
	std::cout << "newConnetion(): accepted a new connection from " << peerAddr.toHostPort() << std::endl;
	::write(sockfd, "How are you?\n", 13);
	muduo::net::sockets::close(sockfd);
}

void newConnetion2(int sockfd, const muduo::net::InetAddress& peerAddr)
{
	std::cout << "newConnetion(): accepted a new connection from " << peerAddr.toHostPort() << std::endl;
	::write(sockfd, "Hello!\n", 7);
	muduo::net::sockets::close(sockfd);
}

int main()
{
	std::cout << "main(): pid = " << getpid() << std::endl;

	muduo::net::InetAddress listenAddr1(9981);
	muduo::net::InetAddress listenAddr2(9982);
	muduo::net::EventLoop loop;

	muduo::net::Acceptor acceptor1(&loop, listenAddr1);
	acceptor1.setNewConnectionCallback(newConnetion1);
	acceptor1.listen();

	muduo::net::Acceptor acceptor2(&loop, listenAddr2);
	acceptor2.setNewConnectionCallback(newConnetion2);
	acceptor2.listen();

	loop.loop();
}
