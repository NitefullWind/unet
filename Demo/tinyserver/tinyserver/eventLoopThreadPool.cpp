#include <tinyserver/eventLoopThreadPool.h>
#include <tinyserver/eventLoopThread.h>

#include <assert.h>

using namespace tinyserver;

EventLoopThreadPool::EventLoopThreadPool(size_t numThreads) :
	_numThreads(numThreads),
	_started(false)
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
		loopThread->startLoop();
		_threadPool.push_back(std::move(loopThread));
	}
}

