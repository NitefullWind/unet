#include <tinyserver/eventLoopThread.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>

#include <assert.h>

using namespace tinyserver;

EventLoopThread::EventLoopThread() :
	_loop(nullptr),
	_looping(false)
{
}

EventLoopThread::~EventLoopThread()
{
	_thread.join();
}

EventLoop *EventLoopThread::startLoop()
{
	LOG_TRACE(__FUNCTION__);
	assert(_looping == false);
	_looping = true;
	_thread = std::thread(std::bind(&EventLoopThread::threadFunc, this));
	return _loop;
}

void EventLoopThread::threadFunc()
{
	LOG_TRACE(__FUNCTION__ << " looping");
	EventLoop loop;
	_loop = &loop;
	loop.loop();
}
