#include <muduo/net/socket.h>
#include <muduo/net/inetAddress.h>
#include <muduo/net/socketsOps.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>			// bzero()

using namespace muduo;
using namespace muduo::net;

Socket::~Socket()
{
	sockets::close(_sockfd);
}

void Socket::bindAddress(const InetAddress& addr)
{
	sockets::bindOrDie(_sockfd, addr.socketAddrInet());
}

void Socket::listen()
{
	sockets::listenOrDir(_sockfd);
}

int Socket::accept(InetAddress *peeraddr)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	int connfd = sockets::accept(_sockfd, &addr);
	if(connfd >= 0) {
		peeraddr->setSocketAddrInet(addr);
	}
	return connfd;
}

void Socket::setReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}
