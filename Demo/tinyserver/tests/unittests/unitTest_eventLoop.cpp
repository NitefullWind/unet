#include <gtest/gtest.h>

#include <tinyserver/buffer.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/eventLoopThreadPool.h>
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

TEST(TestEventLoop, loop)
{
	Logger::InitByFile("log.props");
	InetAddress address(8888);
	EventLoop loop;
	TcpServer server(&loop, address);
	server.setConnectionCallback(onNewConnection);
	server.setIOThreadNum(2);
	server.start();
	loop.loop();
}
