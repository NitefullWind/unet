#include <tinyserver/eventLoop.h>
#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/http/HttpResponse.h>
#include <tinyserver/http/HttpServer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;
using namespace tinyserver::http;

void onHttpRequest(const HttpRequest &req, const HttpResponse &rsp)
{
	LOG_DEBUG(__FUNCTION__);
}

int main(int argc, char** argv)
{
	EventLoop loop;
	InetAddress address(8080);
	HttpServer httpServer(&loop, address);
	httpServer.setIOThreadNum(0);
	httpServer.setHttpCallback(onHttpRequest);
	httpServer.start();
	loop.loop();
	return 0;
}