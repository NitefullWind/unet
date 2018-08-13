#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include <muduo/net/muduo.h>
#include <muduo/net/tcpConnection.h>

#include <memory>
#include <map>

namespace muduo
{
namespace net
{

class EventLoop;
class Acceptor;
class InetAddress;

class TcpServer
{
public:
	TcpServer(EventLoop *loop, const InetAddress& listenAddr);
	~TcpServer();

	void start();

	void setConnectionCallback(const ConnectionCallback& cb) {
		_connectionCallback = cb;
	}
	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;
	}

private:
	void newConnection(int sockfd, const InetAddress& peerAddr);
	void removeConnection(const TcpConnectionPtr& conn);

	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

	EventLoop *_loop;
	const std::string _name;
	std::unique_ptr<Acceptor> _acceptor;
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	bool _started;
	int _nextConnId;
	ConnectionMap _connections;
};

}
}

#endif // MUDUO_NET_TCPSERVER_H
