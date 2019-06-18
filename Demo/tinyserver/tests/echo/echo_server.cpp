#include <tinyserver/buffer.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/eventLoopThreadPool.h>
#include <tinyserver/inetAddress.h>
#include <tinyserver/logger.h>
#include <tinyserver/tcp/tcpConnection.h>
#include <tinyserver/tcp/tcpServer.h>
#include <tinyserver/types.h>

using namespace tinyserver;

void onNewMessage(const TcpConnectionPtr& tcpConnPtr, Buffer *buffer)
{
	size_t len = buffer->readableBytes();
	TLOG_DEBUG("receive new message from: " << tcpConnPtr->peerAddress().toHostPort()
			<< ", Size: " << len);
	tcpConnPtr->send(buffer);
}

void onNewConnection(const TcpConnectionPtr& tcpConnPtr)
{
	TLOG_TRACE("new connection, local address: " << tcpConnPtr->localAddress().toHostPort()
			<< ", peer address: " << tcpConnPtr->peerAddress().toHostPort());
	tcpConnPtr->setMessageCallback(onNewMessage);
}

int main(int argc, char **argv)
{
	Logger::Init("log.props");
	InetAddress address(8888);
	EventLoop loop;
	TcpServer server(&loop, address);
	server.setConnectionCallback(onNewConnection);
	server.setIOThreadNum(2);
	server.start();
	TLOG_INFO("Listen on:" << 8888);
	TLOG_INFO("Number IO threads:" << 2);
	loop.loop();
	return 0;
}
