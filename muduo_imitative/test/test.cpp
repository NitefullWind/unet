#include <iostream>
#include <muduo/net/eventLoop.h>
#include <muduo/net/eventLoopThread.h>
#include <muduo/net/eventLoopThreadPool.h>

using namespace muduo;
using namespace muduo::net;

int main()
{
	EventLoop loop;
	EventLoopThreadPool pool(&loop);
	pool.setThreadNum(2);
	pool.start();
	while(auto l = pool.getNextLoop()) {
		std::cout << "Loop:" << l << std::endl;
	}
	loop.loop();
}
