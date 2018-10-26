#ifndef TINYSERVER_EVENTLOOPTHREAD_H
#define TINYSERVER_EVENTLOOPTHREAD_H

#include <condition_variable>
#include <mutex>
#include <thread>

namespace tinyserver
{

class EventLoop;

class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();

	EventLoop *startLoop();
private:
	EventLoop *_loop;
	std::thread _thread;
	bool _started;
	std::mutex _mutex;
	std::condition_variable _cv;

	void threadFunc();
};

}

#endif // TINYSERVER_EVENTLOOPTHREAD_H
