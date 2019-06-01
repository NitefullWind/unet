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
	LOG_TRACE(reqInfo);

	if(req.path() == "" or req.path() == "/" or req.path() == "/index.html") {
		rsp->setStatusCode(200);
		rsp->setTextBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>Welcome</title></head><body><h1>Hi, this is the home page.</h1></body></html>");
	} else {
		rsp->setStatusCode(404);
		rsp->setStatusMessage("Not Found");
		rsp->setTextBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>404</title></head><body><h1>404 Not Found</h1></body></html>");
		rsp->setKeepAlive(false);
	}
}

int main(int argc, char** argv)
{
	Logger::InitByFile("log.props");
	EventLoop loop;
	InetAddress address(8080);
	HttpServer httpServer(&loop, address);
	httpServer.setIOThreadNum(2);
	httpServer.setHttpCallback(onHttpRequest);
	httpServer.start();
	loop.loop();
	return 0;
}