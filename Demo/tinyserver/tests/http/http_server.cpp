#include <tinyserver/eventLoop.h>
#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/http/HttpResponse.h>
#include <tinyserver/http/HttpServer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;
using namespace tinyserver::http;

#include <iostream>
#include <fstream>
#include <sys/resource.h>
#define CORE_SIZE   1024 * 1024 * 500

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
	TLOG_TRACE(reqInfo);

	std::string reqPath = req.path();
	if(reqPath.back() == '/') {
		reqPath += "index.html";
	}
	if((reqPath.length() > 5 && reqPath.substr(reqPath.length() - 5) == ".html")
	 || (reqPath.length() > 4 && reqPath.substr(reqPath.length() - 4) == ".txt")) {
		reqPath = "html"+reqPath;
		std::ifstream infile(reqPath);
		if(infile.is_open()) {
			std::string data((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
			rsp->setStatusCode(200);
			rsp->setTextBody(data);
		} else {
			rsp->setStatusCode(404);
			rsp->setStatusMessage("Not Found");
			rsp->setTextBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>404</title></head><body><h1>404 Not Found</h1></body></html>");
			rsp->setKeepAlive(false);
		}
		infile.close();
	} else if(reqPath == "/echo") {
		std::string content = req.query("content");
		if(content == "") {
			rsp->setTextBody("Please use: echo?content=echoContent");
		} else {
			rsp->setTextBody(content);
		}
	} else {
		rsp->setStatusCode(404);
		rsp->setStatusMessage("Not Found");
		rsp->setTextBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>404</title></head><body><h1>404 Not Found</h1></body></html>");
		rsp->setKeepAlive(false);
	}
}

int main(int argc, char** argv)
{
	Logger::Init("log.props");
    struct rlimit rlmt;
    rlmt.rlim_cur = (rlim_t)CORE_SIZE;
    rlmt.rlim_max  = (rlim_t)CORE_SIZE;
	if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
		return -1; 
	} 
	EventLoop loop;
	InetAddress address(8080);
	HttpServer httpServer(&loop, address);
	httpServer.setIOThreadNum(2);
	httpServer.setHttpCallback(onHttpRequest);
	httpServer.start();
	loop.loop();
	return 0;
}