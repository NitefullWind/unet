#ifndef MUDUO_NET_TIMERID_H
#define MUDUO_NET_TIMERID_H

#include <cstdint>
#include <stdio.h>

namespace muduo
{
namespace net
{

class Timer;

class TimerId
{
public:
	TimerId():
		_timer(NULL),
		_sequence(0)
	{
	}
	TimerId(Timer *timer, int64_t seq):
		_timer(timer),
		_sequence(seq)
	{
	}

	friend class TimerQueue;
private:
	Timer *_timer;
	int64_t _sequence;
};

}
}

#endif // MUDUO_NET_TIMERID_H
