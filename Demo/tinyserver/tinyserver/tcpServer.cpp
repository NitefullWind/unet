#include <tinyserver/tcpServer.h>
#include <tinyserver/channel.h>
#include <tinyserver/tcpConnection.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/sockets.h>

#include <iostream>

using namespace tinyserver;

TcpServer::TcpServer(EventLoop *loop, const InetAddress& inetAddress) :
	_loop(loop),
	_inetAddress(inetAddress),
	_channel(new Channel(loop, sockets::CreateNonblockingSocket()))
{
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
	_channel->setReadCallback(std::bind(&TcpServer::onNewConnection, this));
	_channel->enableReading();

	sockets::Bind(_channel->fd(), _inetAddress.sockAddrInet());
	sockets::Listen(_channel->fd());
}

void TcpServer::setConnectionCallback(const NewConnectionCallback& cb)
{
	_newConnectionCallback = cb;
}

void TcpServer::onNewConnection()
{
	struct sockaddr_in clientSockaddr;
	int clientfd = sockets::Accept(_channel->fd(), &clientSockaddr);
	if(clientfd > 0) {
		_connections.push_back(std::unique_ptr<TcpConnection>(new TcpConnection(_loop, clientfd)));
 		if(_newConnectionCallback) {
			InetAddress clientAddr(clientSockaddr);
			_newConnectionCallback(clientAddr);
		}
	}
}
