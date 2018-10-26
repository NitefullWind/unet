#ifndef TINYSERVER_EVENTLOOPTHREADPOOL
#define TINYSERVER_EVENTLOOPTHREADPOOL

#include <memory>
#include <vector>

namespace tinyserver
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop *baseLoop, size_t numThreads = 0);
	~EventLoopThreadPool();

	void setThreadNum(size_t numThreads) { _numThreads = numThreads; }
	void start();

	EventLoop *getNextLoop();
private:
	EventLoop *_baseLoop;
	std::vector<std::unique_ptr<EventLoopThread> > _threadPool;
	std::vector<EventLoop *> _loopVector;
	size_t _numThreads;
	bool _started;
	size_t _loopIndex;
};

}

#endif // TINYSERVER_EVENTLOOPTHREADPOOL
