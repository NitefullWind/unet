#include <muduo/net/acceptor.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/inetAddress.h>
#include <muduo/net/socketsOps.h>

#include <functional>

using namespace muduo;
using namespace muduo::net;

Acceptor::Acceptor(EventLoop *loop, const InetAddress& listenAddr) :
	_loop(loop),
	_acceptSocket(sockets::createNonblockingOrDie()),
	_acceptChannel(loop, _acceptSocket.fd()),
	_listenning(false)
{
	_acceptSocket.setReuseAddr(true);
	_acceptSocket.bindAddress(listenAddr);
	_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen()
{
	_loop->assertInLoopThread();
	_listenning = true;
	_acceptSocket.listen();
	_acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
	_loop->assertInLoopThread();
	InetAddress peerAddr(0);
	int connfd = _acceptSocket.accept(&peerAddr);
	if(connfd >= 0) {
		if(_newConnectionCallback) {
			_newConnectionCallback(connfd, peerAddr);
		} else {
			sockets::close(connfd);
		}
	}
}
