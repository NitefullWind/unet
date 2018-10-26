#include <tinyserver/eventLoopThreadPool.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/eventLoopThread.h>

#include <assert.h>

using namespace tinyserver;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, size_t numThreads) :
	_baseLoop(baseLoop),
	_numThreads(numThreads),
	_started(false),
	_loopIndex(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start()
{
	assert(_started == false);
	_started = true;

	for(size_t i=0; i<_numThreads; ++i) {
		std::unique_ptr<EventLoopThread> loopThread(new EventLoopThread());
		_loopVector.push_back(loopThread->startLoop());
		_threadPool.push_back(std::move(loopThread));
	}
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
	EventLoop *loop = _baseLoop;
	if(!_loopVector.empty()) {
		loop = _loopVector[_loopIndex];
		_loopIndex++;
		if(_loopIndex >= _loopVector.size()) {
			_loopIndex = 0;
		}
	}
	return loop;
}
