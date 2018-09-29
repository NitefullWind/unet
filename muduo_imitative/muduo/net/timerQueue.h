#ifndef MUDUO_NET_TIMEQUEUE_H
#define MUDUO_NET_TIMEQUEUE_H

#include <muduo/base/timestamp.h>
#include <muduo/net/muduo.h>
#include <muduo/net/channel.h>
#include <vector>
#include <set>

namespace muduo
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue
{
public:
	TimerQueue(EventLoop *loop);
	~TimerQueue();

	TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

	void cancel(TimerId timerId);
private:
	typedef std::pair<Timestamp, Timer*> Entry;
	typedef std::set<Entry> TimerList;
	typedef std::pair<Timer*, int64_t> ActiveTimer;
	typedef std::set<ActiveTimer> ActiveTimerSet;

	void addTimerInLoop(Timer *timer);
	void cancelInLoop(TimerId timerId);
	// called when expired timers
	void handleRead();

	// move out all expired timers
	std::vector<Entry> getExpired(Timestamp now);
	void reset(std::vector<Entry>& expired, Timestamp now);

	bool insert(Timer *timer);

	EventLoop *_loop;
	const int _timerfd;
	Channel _timerfdChannel;
	// Timer list sorted by expiration
	TimerList _timers;

	// for cancel
	bool _callingExpiredTimers;
	ActiveTimerSet _activeTimers;
	ActiveTimerSet _cancelingTimers;
};

}
}

#endif // MUDUO_NET_TIMEQUEUE_H
