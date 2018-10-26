#include <tinyserver/tcpServer.h>
#include <tinyserver/channel.h>
#include <tinyserver/tcpConnection.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/eventLoopThreadPool.h>
#include <tinyserver/sockets.h>

#include <iostream>

using namespace tinyserver;

TcpServer::TcpServer(EventLoop *loop, const InetAddress& inetAddress) :
	_loop(loop),
	_inetAddress(inetAddress),
	_channel(new Channel(loop, sockets::CreateNonblockingSocket())),
	_connectionCounter(0),
	_IOThreadPool(new EventLoopThreadPool(loop))
{
}

TcpServer::~TcpServer()
{
	LOG_TRACE(__FUNCTION__);
	sockets::Close(_channel->fd());
}

void TcpServer::start()
{
	_IOThreadPool->start();
	_channel->setReadCallback(std::bind(&TcpServer::onNewConnection, this));
	_channel->enableReading();

	sockets::Bind(_channel->fd(), _inetAddress.sockAddrInet());
	sockets::Listen(_channel->fd());
}

void TcpServer::onNewConnection()
{
	LOG_TRACE(__FUNCTION__);
	struct sockaddr_in clientSockaddr;
	int clientfd = sockets::Accept(_channel->fd(), &clientSockaddr);
	if(clientfd >= 0) {
		TcpConnectionPtr tcpConnPtr(new TcpConnection(_IOThreadPool->getNextLoop(), clientfd));
		tcpConnPtr->setIndex(_connectionCounter);
		++_connectionCounter;
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

void TcpServer::setIOThreadNum(size_t numThreads)
{
	_IOThreadPool->setThreadNum(numThreads);
}
