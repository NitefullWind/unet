#include <tinyserver/tcp/tcpServer.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/eventLoopThreadPool.h>
#include <tinyserver/sockets.h>
#include <tinyserver/tcp/tcpConnection.h>

#include <iostream>

using namespace tinyserver;

TcpServer::TcpServer(EventLoop *loop, const InetAddress& inetAddress) :
	_loop(loop),
	_inetAddress(inetAddress),
	_channel(new Channel(loop, sockets::CreateNonblockingSocket())),
	_nextConnId(1),
	_IOThreadPool(new EventLoopThreadPool(loop))
{
}

TcpServer::~TcpServer()
{
	TLOG_TRACE(__FUNCTION__);
	sockets::Close(_channel->fd());
}

void TcpServer::start()
{
	_IOThreadPool->start();
	_channel->setReadCallback(std::bind(&TcpServer::onNewConnection, this));
	_channel->enableReading();

	sockets::Bind(_channel->fd(), _inetAddress.sockAddrInet());
	sockets::Listen(_channel->fd());
	sockets::SetReuseAddr(_channel->fd(), true);
	sockets::SetReusePort(_channel->fd(), true);
}

void TcpServer::setIOThreadNum(size_t numThreads)
{
	_IOThreadPool->setThreadNum(numThreads);
}

void TcpServer::onNewConnection()
{
	TLOG_TRACE(__FUNCTION__);
	struct sockaddr_in clientSockaddr;
	int clientfd = sockets::Accept(_channel->fd(), &clientSockaddr);
	if(clientfd >= 0) {
		EventLoop *ioLoop = _IOThreadPool->getNextLoop();
		char buf[64];
		snprintf(buf, sizeof buf, "%d_%d", sockets::NetworkToHost16(clientSockaddr.sin_port), _nextConnId);
		++_nextConnId;
		TcpConnectionPtr tcpConnPtr(new TcpConnection(ioLoop, buf, clientfd));
		tcpConnPtr->setConnectionCallback(_newConnectionCallback);
		tcpConnPtr->setDisonnectionCallback(_disconnectionCallback);
		tcpConnPtr->setMessageCallback(_messageCallback);
		tcpConnPtr->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
		_connectionMap[tcpConnPtr->id()] = tcpConnPtr;
		ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished, tcpConnPtr));
	}
}

void TcpServer::removeConnection(const TcpConnectionPtr& tcpConnPtr)
{
	_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, tcpConnPtr));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& tcpConnPtr)
{
	_loop->assertInLoopThread();
	std::map<std::string, TcpConnectionPtr>::iterator it = _connectionMap.find(tcpConnPtr->id());
	if(it != _connectionMap.end()) {
		_connectionMap.erase(it);
	}
	EventLoop *ioLoop = tcpConnPtr->ownerLoop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, tcpConnPtr));
}
