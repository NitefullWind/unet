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

void TcpServer::onNewConnection()
{
	struct sockaddr_in clientSockaddr;
	int clientfd = sockets::Accept(_channel->fd(), &clientSockaddr);
	if(clientfd >= 0) {
		TcpConnectionPtr tcpConnPtr(new TcpConnection(_loop, clientfd));
		tcpConnPtr->setIndex(_connectionMap.size());
		tcpConnPtr->setMessageCallback(_messageCallback);
		tcpConnPtr->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
		_connectionMap[tcpConnPtr->index()] = tcpConnPtr;
 		if(_newConnectionCallback) {
			_newConnectionCallback(tcpConnPtr);
		}
	}
}

void TcpServer::removeConnection(const TcpConnectionPtr& tcpConnPtr)
{
	LOG_TRACE(__FUNCTION__);
	_connectionMap.erase(_connectionMap.find(tcpConnPtr->index()));
}
