#include <tinyserver/http/HttpServer.h>
#include <tinyserver/logger.h>
#include <tinyserver/tcp/tcpConnection.h>
#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/http/HttpResponse.h>
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
	HttpRequest request;
	request.parserRequest(buffer);
	std::string connection = request.header("Connection");
	HttpResponse response;
	// 根据Connection请求头的值判断是否是长连接，其值为keep-alive时，或者Http1.1中其值不为close则该请求是长连接
	if(connection == "keep-alive" || (request.version() == HttpRequest::Version::Http11 && connection != "close")) {
		response.setKeepAlive(true);
	}
	if(_httpCallback) {
		_httpCallback(request, &response);
	}
	
	Buffer responseBuffer;
	response.getResponseBuffer(&responseBuffer);
	tcpConnPtr->send(&responseBuffer);

	if(!response.keepAlive()) {
		tcpConnPtr->shutdown();
	}
}