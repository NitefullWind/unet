#ifndef MUDUO_NET_EVENTLOOPTHREADPOOL_H
#define MUDUO_NET_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>
#include <functional>

namespace muduo
{
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop *baseLoop);
	~EventLoopThreadPool();
	void setThreadNum(int numThreads) { _numThreads = numThreads; }
	void start();
	EventLoop *getNextLoop();

private:
	EventLoop *_baseLoop;
	bool _started;
	int _numThreads;
	int _next;
	std::vector<std::unique_ptr<EventLoopThread> > _threads;
	std::vector<EventLoop*> _loops;
};

}
}

#endif // MUDUO_NET_EVENTLOOPTHREADPOOL_H
