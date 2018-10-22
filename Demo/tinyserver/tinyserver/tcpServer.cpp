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
		std::shared_ptr<TcpConnection> tcpConn(new TcpConnection(_loop, clientfd));
		tcpConn->setIndex(_connectionMap.size());
		tcpConn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
		_connectionMap[tcpConn->index()] = tcpConn;
 		if(_newConnectionCallback) {
			InetAddress clientAddr(clientSockaddr);
			_newConnectionCallback(clientAddr);
		}
	}
}

void TcpServer::removeConnection(size_t index)
{
	LOG_TRACE(__FUNCTION__);
	_connectionMap.erase(_connectionMap.find(index));
}
