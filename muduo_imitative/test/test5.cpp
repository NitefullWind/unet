#include <muduo/net/eventLoop.h>
#include <muduo/net/eventLoopThread.h>
#include <thread>
#include <unistd.h>
#include <iostream>

void runInThread()
{
	std::cout << "runInThread(): pid = " << getpid() << ", tid = " << std::this_thread::get_id() << std::endl;
}

int main()
{
	std::cout << "main(): pid = " << getpid() << ", tid = " << std::this_thread::get_id() << std::endl;

	muduo::net::EventLoopThread loopThread;
	muduo::net::EventLoop *loop = loopThread.eventLoop();
	loop->runInLoop(runInThread);			// 跨线程调用
	sleep(1);
	loop->runAfter(2, runInThread);			// 跨线程调用
	sleep(3);
	loop->quit();							// 时间循环结束后，在栈上创建的loop自动析构

	std::cout << "exit main().\n";
	
	return 0;
}
