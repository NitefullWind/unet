#include <muduo/net/eventLoop.h>
#include <muduo/base/logger.h>
#include <assert.h>
#include <poll.h>

using namespace muduo;
using namespace muduo::net;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop() :
	_looping(false),
	_tid(std::this_thread::get_id())
{
	LOG_TRACE("EventLoop created " << this << " in thread " << _tid);
	if(t_loopInThisThread) {
		LOG_FATAL("Another EventLoop " << t_loopInThisThread
				<< " exists in this thread " << _tid);
	} else {
		t_loopInThisThread = this;
	}
}

EventLoop::~EventLoop()
{
	assert(!_looping);
	t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
	assert(!_looping);
	assertInLoopThread();
	_looping = true;

	::poll(NULL, 0, 5*1000);

	LOG_TRACE("EventLoop " << this << " stop looping");
	_looping = false;
}

void EventLoop::assertInLoopThread()
{
	if(!isInLoopThread()) {
		abortNotInLoopThread();
	}
}

inline bool EventLoop::isInLoopThread() const
{
	return (_tid == std::this_thread::get_id());
}

void EventLoop::abortNotInLoopThread()
{
	LOG_FATAL(__FUNCTION__ << " EventLoop " << this
			<< " was created in thread id = " << _tid
			<< ", current thread id = " << std::this_thread::get_id());
}
