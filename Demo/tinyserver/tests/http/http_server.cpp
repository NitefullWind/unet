#include <tinyserver/eventLoop.h>
#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/http/HttpResponse.h>
#include <tinyserver/http/HttpServer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;
using namespace tinyserver::http;

#include <iostream>

void onHttpRequest(const HttpRequest &req, HttpResponse* rsp)
{
	std::string reqInfo;
	reqInfo.append("\n=====" + req.versionString() + " " + req.methodString() + "=====\n");
	reqInfo.append("=====HEADERS=====\n");
	for (auto &&v : req.headers())
	{
		reqInfo.append("===" + v.first + ": " + v.second + "\n");
	}
	reqInfo.append("=====PATH: " + req.path() + " =====\n");
	reqInfo.append("=====QUERIES=====\n");
	for (auto &&v : req.queries())
	{
		reqInfo.append("===" + v.first + ": " + v.second + "\n");
	}
	reqInfo.append("=====BODY=====\n" + req.body() + "\n================================\n");
	LOG_DEBUG(reqInfo);

	rsp->setStatusCode(200);
	rsp->setHeader("Content-Length", "0");
	rsp->setContentType("text/html");

}

int main(int argc, char** argv)
{
	Logger::InitByFile("log.props");
	EventLoop loop;
	InetAddress address(8080);
	HttpServer httpServer(&loop, address);
	httpServer.setIOThreadNum(0);
	httpServer.setHttpCallback(onHttpRequest);
	httpServer.start();
	loop.loop();
	return 0;
}