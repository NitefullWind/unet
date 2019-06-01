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
	LOG_TRACE("A new http connection from "<< tcpConnPtr->peerAddress().toHostPort());
}

void HttpServer::onMessage(const TcpConnectionPtr &tcpConnPtr, Buffer* buffer)
{
	HttpRequest request;
	HttpResponse response;
	if(request.parserRequest(buffer)) {
		std::string connection = request.header("Connection");
		// 根据Connection请求头的值判断是否是长连接，其值为keep-alive时，或者Http1.1中其值不为close则该请求是长连接
		if(connection == "keep-alive" || (request.version() == HttpRequest::Version::Http11 && connection != "close")) {
			response.setKeepAlive(true);
		}
	} else {
		response.setStatusCode(400);
		response.setStatusMessage("Bad Request");
		response.setTextBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>400</title></head><body><h1>400 Bad Request</h1></body></html>");
		response.setKeepAlive(false);
	}

	if(_httpCallback) {
		_httpCallback(request, &response);
	}

	LOG_DEBUG(tcpConnPtr->peerAddress().toHostPort() << " " << request.methodString() << " " << request.path() << " " << request.versionString() << " " << response.statusCode());
	
	Buffer responseBuffer;
	response.getResponseBuffer(&responseBuffer);
	tcpConnPtr->send(&responseBuffer);

	if(!response.keepAlive()) {
		tcpConnPtr->shutdown();
	}
}