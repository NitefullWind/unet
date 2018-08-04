#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <thread>
#include <vector>
#include <memory>
#include <muduo/net/muduo.h>
#include <muduo/base/timestamp.h>

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
	EventLoop();
	~EventLoop();

	void loop();

	void quit();

	void runAt(const Timestamp& time, const TimerCallback& cb);
	void runAfter(double delay, const TimerCallback& cb);
	void runEvery(double interval, const TimerCallback& cb);

	void updateChannel(Channel *channel);

	void assertInLoopThread();

	bool isInLoopThread() const;
private:
	void abortNotInLoopThread();

	typedef std::vector<Channel *> ChannelList;

	bool _looping;
	bool _quit;
	const std::thread::id _tid;
	std::unique_ptr<Poller> _poller;
	std::unique_ptr<TimerQueue> _timerQueue;
	ChannelList _activeChannels;
};

}
}

#endif // MUDUO_NET_EVENTLOOP_H
