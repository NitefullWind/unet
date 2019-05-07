#ifndef TINYSERVER_HTTPSERVER_H
#define TINYSERVER_HTTPSERVER_H

#include <tinyserver/tcp/tcpServer.h>

namespace tinyserver
{

class EventLoop;
class InetAddress;

namespace http
{

class HttpServer
{
public:
	explicit HttpServer(EventLoop* loop, const InetAddress &addr);
	~HttpServer();

	void start() { _tcpServer.start(); }

	void setIOThreadNum(size_t numThreads) { _tcpServer.setIOThreadNum(numThreads); }

	void setHttpCallback(const HttpCallback &cb) { _httpCallback = cb; }
private:
	TcpServer _tcpServer;
	HttpCallback _httpCallback;

	void onConnection(const TcpConnectionPtr &tcpConnPtr);
	void onMessage(const TcpConnectionPtr &tcpConnPtr, Buffer* buffer);
};

}
}

#endif // TINYSERVER_HTTPSERVER_H