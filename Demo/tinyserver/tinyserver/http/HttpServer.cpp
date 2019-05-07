#include <tinyserver/http/HttpServer.h>
#include <tinyserver/logger.h>
#include <tinyserver/tcp/tcpConnection.h>
#include <functional>

using namespace tinyserver;
using namespace tinyserver::http;

HttpServer::HttpServer(EventLoop *loop, const InetAddress& addr):
	_tcpServer(loop, addr)
{
	_tcpServer.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
	_tcpServer.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer()
{

}
void HttpServer::onConnection(const TcpConnectionPtr &tcpConnPtr)
{
	LOG_DEBUG("A new http connection from "<< tcpConnPtr->peerAddress().toHostPort());
}

void HttpServer::onMessage(const TcpConnectionPtr &tcpConnPtr, Buffer* buffer)
{
	LOG_DEBUG(__FUNCTION__ << tcpConnPtr->peerAddress().toHostPort() << " : " << buffer->readAll());
	tcpConnPtr->shutdown();
}