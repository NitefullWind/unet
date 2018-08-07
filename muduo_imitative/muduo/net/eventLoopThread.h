#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include <thread>
#include <condition_variable>

namespace muduo
{
namespace net
{

class EventLoop;

/*
 * 与muduo中的EventThread类不同之处在于，这里在创建EventLoopThread对象后立即开始事件循环 
 */
class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();

	// 此接口仅返回EventLoop对象，不调用其loop()函数
	EventLoop *eventLoop();

private:
	void threadFunc();

	EventLoop *_loop;
	bool _exiting;
	std::mutex _mutex;
	std::condition_variable _conn;
	std::thread _thread;
};

}
}

#endif // MUDUO_NET_EVENTLOOPTHREAD_H

