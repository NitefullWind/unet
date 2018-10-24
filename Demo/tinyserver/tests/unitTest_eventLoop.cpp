#include <gtest/gtest.h>

#include <tinyserver/buffer.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/inetAddress.h>
#include <tinyserver/logger.h>
#include <tinyserver/tcpConnection.h>
#include <tinyserver/tcpServer.h>
#include <tinyserver/types.h>

using namespace tinyserver;

void onNewMessage(const TcpConnectionPtr& tcpConnPtr, Buffer *buffer)
{
	size_t len = buffer->readableBytes();
	LOG_DEBUG("receive new message from: " << tcpConnPtr->peerAddress().toHostPort()
			<< ", Size: " << len);
	tcpConnPtr->send(buffer);
}

void onNewConnection(const TcpConnectionPtr& tcpConnPtr)
{
	LOG_TRACE("new connection, local address: " << tcpConnPtr->localAddress().toHostPort()
			<< ", peer address: " << tcpConnPtr->peerAddress().toHostPort());
	tcpConnPtr->setMessageCallback(onNewMessage);
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
