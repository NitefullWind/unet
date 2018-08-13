#include <muduo/net/tcpServer.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/acceptor.h>
#include <muduo/net/inetAddress.h>
#include <muduo/net/socketsOps.h>
#include <muduo/base/logger.h>

#include <stdio.h>					// snprintf()
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr):
	_loop(loop),
	_name(listenAddr.toHostPort()),
	_acceptor(new Acceptor(loop, listenAddr)),
	_started(false),
	_nextConnId(1)
{
	_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
	if(!_started) {
		_started = true;
	}
	if(!_acceptor->listening()) {
		_loop->runInLoop(std::bind(&Acceptor::listen, _acceptor.get()));
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	_loop->assertInLoopThread();
	char buf[32];
	snprintf(buf, sizeof(buf), "#%d", _nextConnId);
	++_nextConnId;
	std::string connName = _name + buf;

	LOG_INFO("TcpServer::newConnection [" << _name << "] - new conntion [" 
			<< connName << "] from " << peerAddr.toHostPort());
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn = 
		std::make_shared<TcpConnection>(_loop, connName, sockfd, localAddr, peerAddr);
	_connections[connName] = conn;
	conn->setConnectionCallback(_connectionCallback);
	conn->setMessageCallback(_messageCallback);
	conn->setCloseCallback(
			std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
	conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	_loop->assertInLoopThread();
	LOG_INFO("TcpServer::removeConnection [" << _name << "] - connection "
			<<  conn->name());
	size_t n = _connections.erase(conn->name());
	assert(n == 1); (void)n;
	_loop->queueInLoop(
			std::bind(&TcpConnection::connectDestroyed, conn));
}
