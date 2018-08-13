#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <muduo/net/muduo.h>
#include <muduo/base/timestamp.h>
#include <muduo/net/timerId.h>

namespace muduo
{
namespace net
{

class Channel;
class Poller;
class TimerQueue;

class EventLoop
{
public:
	typedef std::function<void()> Functor;

	EventLoop();
	~EventLoop();

	void loop();

	void quit();

	void runInLoop(const Functor& cb);

	void queueInLoop(const Functor& cb);

	TimerId runAt(const Timestamp& time, const TimerCallback& cb);
	TimerId runAfter(double delay, const TimerCallback& cb);
	TimerId runEvery(double interval, const TimerCallback& cb);

	void wakeup();
	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

	void assertInLoopThread();

	bool isInLoopThread() const;
private:
	void abortNotInLoopThread();
	void handleRead();					// waked up
	void doPendingFunctors();

	typedef std::vector<Channel *> ChannelList;

	bool _looping;
	bool _quit;
	bool _callingPendingFunctors;
	const std::thread::id _tid;
	std::unique_ptr<Poller> _poller;
	std::unique_ptr<TimerQueue> _timerQueue;
	int _wakeupFd;
	std::unique_ptr<Channel> _wakeupChannel;
	ChannelList _activeChannels;
	std::mutex _mutex;
	std::vector<Functor> _pendingFunctors;	// @GuardedBy _mutex
};

}
}

#endif // MUDUO_NET_EVENTLOOP_H
