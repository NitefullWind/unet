#include <muduo/net/eventLoopThreadPool.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/eventLoopThread.h>
#include <assert.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop):
	_baseLoop(baseLoop),
	_started(false),
	_numThreads(0),
	_next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
	// Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start()
{
	assert(!_started);
	_baseLoop->assertInLoopThread();

	_started = true;

	for(int i=0; i<_numThreads; ++i) {
		std::unique_ptr<EventLoopThread> t(new EventLoopThread());
		_loops.push_back(t->eventLoop());	// 和下面一行顺序不能换，因为move使t为null了
		_threads.push_back(std::move(t));
	}
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
	_baseLoop->assertInLoopThread();
	EventLoop *loop = _baseLoop;

	if(!_loops.empty()) {
		loop = _loops[_next];
		++_next;
		if(static_cast<size_t>(_next) >= _loops.size()) {
			_next = 0;
		}
	}

	return loop;
}


