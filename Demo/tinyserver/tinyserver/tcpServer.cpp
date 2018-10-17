#include <tinyserver/eventLoop.h>
#include <tinyserver/tcpServer.h>
#include <tinyserver/sockets.h>

using namespace tinyserver;

TcpServer::TcpServer(EventLoop *loop, const InetAddress& inetAddress) :
	_loop(loop),
	_inetAddress(inetAddress),
	_sockfd(0)
{
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
	_sockfd = sockets::CreateNonblockingSocket();
	_loop->addSockets(_sockfd);
	sockets::Bind(_sockfd, _inetAddress.sockAddrInet());
	sockets::Listen(_sockfd);
}
