/**
 * test TimerQueue
 */
#include <muduo/net/eventLoop.h>
#include <muduo/base/logger.h>
#include <unistd.h>
#include <thread>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

int cnt = 0;
EventLoop* g_loop;

void printTid()
{
	std::cout << "pid = " << getpid() << ", tid = " << std::this_thread::get_id() << std::endl;
	std::cout << "now " << Timestamp::now().toString() << std::endl;
}

void print(const char *msg)
{
	std::cout << "msg " << Timestamp::now().toString() << " " << msg << std::endl;
	if(++cnt == 20) {
		g_loop->quit();
	}
}

int main()
{
	printTid();
	EventLoop loop;
	g_loop = &loop;

	print("main");
	loop.runAfter(1, std::bind(print, "once1"));
	loop.runAfter(1.5, std::bind(print, "once1.5"));
	loop.runAfter(2.5, std::bind(print, "once2.5"));
	loop.runAfter(3.5, std::bind(print, "once3.5"));
	loop.runEvery(2, std::bind(print, "every2"));
	loop.runEvery(3, std::bind(print, "every3"));

	loop.loop();
	print("main loop exits");
	sleep(1);
	return 0;
}
