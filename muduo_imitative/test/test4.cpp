/*
 * test runInLoop() and queueInLoop()
 */
#include <iostream>
#include <muduo/net/eventLoop.h>
#include <thread>
#include <unistd.h>

muduo::net::EventLoop *g_loop;
int g_flag = 0;

using namespace std;

void run4()
{
	cout << "run4(): pid = " << getpid() << ", flag = " << g_flag << endl;
	g_loop->quit();
}

void run3()
{
	cout << "run3(): pid = " << getpid() << ", flag = " << g_flag << endl;
	g_loop->runAfter(3, run4);
	g_flag = 3;
}

void run2()
{
	cout << "run2(): pid = " << getpid() << ", flag = " << g_flag << endl;
	g_loop->queueInLoop(run3);
}

void run1()
{
	g_flag = 1;
	cout << "run1(): pid = " << getpid() << ", flag = " << g_flag << endl;
	g_loop->runInLoop(run2);
	g_flag = 2;
}

int main()
{
	cout << "main(): pid = " << getpid() << ", flag = " << g_flag << endl;

	muduo::net::EventLoop loop;
	g_loop = &loop;

	loop.runAfter(2, run1);
	loop.loop();

	cout << "main(): pid = " << getpid() << ", flag = " << g_flag << endl;
	return 0;
}
