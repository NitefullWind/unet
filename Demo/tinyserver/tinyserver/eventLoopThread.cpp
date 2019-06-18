#include <tinyserver/eventLoopThread.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>

#include <assert.h>

using namespace tinyserver;

EventLoopThread::EventLoopThread() :
	_loop(nullptr),
	_started(false)
{
}

EventLoopThread::~EventLoopThread()
{
	_thread.join();
}

EventLoop *EventLoopThread::startLoop()
{
	TLOG_TRACE(__FUNCTION__);
	assert(_started == false);
	_started = true;
	_thread = std::thread(std::bind(&EventLoopThread::threadFunc, this));
	{
		std::unique_lock<std::mutex> lk(_mutex);
		_cv.wait(lk, [&]{ return _loop != nullptr; });		// if loop == nullptr then wait
	}
	return _loop;
}

void EventLoopThread::threadFunc()
{
	TLOG_TRACE(__FUNCTION__ << " looping");
	EventLoop loop;
	{
		std::lock_guard<std::mutex> lk(_mutex);
		_loop = &loop;
		_cv.notify_one();
	}
	loop.loop();
}
