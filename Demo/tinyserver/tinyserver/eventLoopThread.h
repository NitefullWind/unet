#ifndef TINYSERVER_EVENTLOOPTHREAD_H
#define TINYSERVER_EVENTLOOPTHREAD_H

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
	bool _looping;

	void threadFunc();
};

}

#endif // TINYSERVER_EVENTLOOPTHREAD_H
