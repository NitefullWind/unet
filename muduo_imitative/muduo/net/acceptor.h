#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <functional>

#include <muduo/net/channel.h>
#include <muduo/net/socket.h>

namespace muduo
{
namespace net
{

class Acceptor
{
public:
	typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop *loop, const InetAddress& listenAddr);

	void setNewConnectionCallback(const NewConnectionCallback& cb) {
		_newConnectionCallback = cb;
	}

	bool listening() const { return _listenning; }
	void listen();

private:
	void handleRead();

	EventLoop *_loop;
	Socket _acceptSocket;
	Channel _acceptChannel;
	NewConnectionCallback _newConnectionCallback;
	bool _listenning;
};

}
}

#endif // MUDUO_NET_ACCEPTOR_H
