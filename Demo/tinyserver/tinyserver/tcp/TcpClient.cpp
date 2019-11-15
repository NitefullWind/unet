#include <tinyserver/tcp/TcpClient.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>
#include <tinyserver/tcp/Connector.h>
#include <tinyserver/tcp/tcpConnection.h>

#include <assert.h>

using namespace tinyserver;

TcpClient::TcpClient(EventLoop *loop, const InetAddress& serverAddress):
	_loop(loop),
	_connector(new Connector(loop, serverAddress)),
	_nextConnId(1),
	_retry(false),
	_connect(false)
{
	_connector->setNewConnectionCallback(std::bind(&TcpClient::onNewConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient()
{
	TcpConnectionPtr conn;
	{
		std::lock_guard<std::mutex> lk(_mutex);
		conn = _tcpConnection;
	}
	if(conn) {	// 已连接则关闭连接
		conn->shutdown();
	} else {	// 未连接尝试停止连接
		_connector->stop(); 
	}
}

void TcpClient::connect()
{
	_connect = true;
	_connector->start();
}

void TcpClient::disconnect()
{
	_connect = false;
	{
		std::lock_guard<std::mutex> lk(_mutex);
		if(_tcpConnection) {
			_tcpConnection->shutdown();
		}
	}
}

void TcpClient::stop()
{
	_connect = false;
	_connector->stop();
}

std::shared_ptr<TcpConnection> TcpClient::tcpConnection() const
{
	return _tcpConnection;
}

void TcpClient::onNewConnection(int sockfd)
{
	_loop->assertInLoopThread();
	InetAddress peerAddr(sockets::GetPeerAddr(sockfd));
	char buf[64];
	snprintf(buf, sizeof buf, "%s_%d", peerAddr.toHostPort().c_str(), _nextConnId);
	++_nextConnId;
	InetAddress localAddr(sockets::GetLocalAddr(sockfd));
	TcpConnectionPtr tcpConnPtr(new TcpConnection(_loop, buf, sockfd));
	tcpConnPtr->setConnectionCallback(_connectionCallback);
	tcpConnPtr->setMessageCallback(_messageCallback);
	tcpConnPtr->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
	{
		std::lock_guard<std::mutex> lk(_mutex);
		_tcpConnection = tcpConnPtr;
	}
	tcpConnPtr->connectionEstablished();
}

void TcpClient::removeConnection(const std::shared_ptr<TcpConnection>& conn)
{
	_loop->assertInLoopThread();
	assert(_loop == conn->ownerLoop());
	{
		std::lock_guard<std::mutex> lk(_mutex);
		assert(_tcpConnection == conn);
		_tcpConnection.reset();
	}
	_loop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
	if(_connect && _retry) { // 断线重连
		TLOG_INFO("Reconnecting to " << _connector->serverAddress().toHostPort());
		_connector->restart();
	}
}