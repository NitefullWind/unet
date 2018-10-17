#include <tinyserver/tcpServer.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/sockets.h>

using namespace tinyserver;

TcpServer::TcpServer(EventLoop *loop, const InetAddress& inetAddress) :
	_loop(loop),
	_inetAddress(inetAddress),
	_channel(new Channel(sockets::CreateNonblockingSocket()))
{
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
	_loop->addChannel(_channel.get());
	sockets::Bind(_channel->fd(), _inetAddress.sockAddrInet());
	sockets::Listen(_channel->fd());
}

void TcpServer::setConnectionCallback(const ConnectionCallback& cb)
{
	_connectCallback = cb;
}
