#include <gtest/gtest.h>

#include <tinyserver/eventLoop.h>
#include <tinyserver/inetAddress.h>
#include <tinyserver/logger.h>
#include <tinyserver/tcpServer.h>

using namespace tinyserver;

void onNewConnection(const InetAddress& address)
{
	LOG_TRACE("new connection from : " << address.toHostPort());
}

TEST(TestEventLoop, Test1)
{
	EXPECT_EQ(1, 1);
}

TEST(TestEventLoop, loop)
{
	InetAddress address(8888);
	EventLoop loop;
	TcpServer server(&loop, address);
	server.setConnectionCallback(onNewConnection);
	server.start();
	loop.loop();
}
