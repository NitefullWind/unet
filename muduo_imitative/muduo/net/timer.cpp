#include <muduo/net/timer.h>

using namespace muduo;
using namespace muduo::net;

std::atomic_int_fast64_t Timer::s_numCreated;

void Timer::restart(Timestamp now)
{
	if(_repeat) {
		_expiration = addTime(now, _interval);
	} else {
		_expiration = Timestamp::invalid();
	}
}
