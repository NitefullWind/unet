#include <muduo/net/eventLoopThread.h>
#include <muduo/net/eventLoop.h>

using namespace muduo;
using namespace muduo::net;

EventLoopThread::EventLoopThread():
	_loop(NULL),
	_exiting(false),
	_mutex(),
	_conn(),
	_thread(std::bind(&EventLoopThread::threadFunc, this))
{
}

EventLoopThread::~EventLoopThread()
{
	_exiting = true;
	_loop->quit();
	_thread.join();
}

EventLoop *EventLoopThread::eventLoop()
{
	{
		std::unique_lock<std::mutex> lock(_mutex);
		while(_loop == NULL) {
			_conn.wait(lock);
		}
	}
	return _loop;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_loop = &loop;
		_conn.notify_one();
	}
	loop.loop();							// 时间循环结束后，在栈上创建的loop自动析构
}
