#ifndef TINYSERVER_EVENTLOOPTHREADPOOL
#define TINYSERVER_EVENTLOOPTHREADPOOL

#include <memory>
#include <vector>

namespace tinyserver
{

class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(size_t numThreads = 0);
	~EventLoopThreadPool();

	void setThreadNum(size_t numThreads) { _numThreads = numThreads; }
	void start();

private:
	std::vector<std::unique_ptr<EventLoopThread> > _threadPool;
	size_t _numThreads;
	bool _started;
};

}

#endif // TINYSERVER_EVENTLOOPTHREADPOOL
