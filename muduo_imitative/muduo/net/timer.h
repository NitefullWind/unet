#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H

#include <muduo/net/muduo.h>
#include <muduo/base/timestamp.h>
#include <atomic>					// atomic_int_fast64_t

namespace muduo
{
namespace net
{

class Timer
{
public:
	Timer(const TimerCallback& cb, Timestamp when, double interval):
		_callback(cb),
		_expiration(when),
		_interval(interval),
		_repeat(interval>0.0),
		_sequence(s_numCreated.fetch_add(1))
	{
	}
	~Timer() {}

	void run() const {
		_callback();
	}

	Timestamp expiration() const { return _expiration; }
	bool repeat() const { return _repeat; }
	int64_t sequence() const { return _sequence; }

	void restart(Timestamp now);

	static int64_t numCreated() { return s_numCreated.load(); }

private:
	const TimerCallback _callback;
	Timestamp _expiration;
	const double _interval;
	const bool _repeat;
	const int64_t _sequence;

	static std::atomic_int_fast64_t s_numCreated;
};

}
}

#endif // MUDUO_NET_TIMER_H
