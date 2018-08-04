#include <muduo/base/logger.h>
#include <muduo/net/timerQueue.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/timerId.h>
#include <muduo/net/timer.h>
#include <sys/timerfd.h>
#include <strings.h>				// bzero()
#include <unistd.h>					// read()

namespace muduo
{
namespace net
{
namespace detail
{

int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

	if(timerfd < 0) {
		LOG_ERROR("Failed in timerfd_create");
	}
	return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
	int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();

	if(microseconds < 100) {
		microseconds = 100;
	}
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
	return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
	uint64_t howmany;
	ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
	LOG_TRACE("TimerQueue::handleRead() " << howmany << " at " << now.toString());
	if(n != sizeof(howmany)) {
		LOG_ERROR("TimerQueue::handleRead() reads " << n << " bytes instead of 8");
	}
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
	// wake up loop by timerfd_settime()
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue, sizeof(newValue));
	bzero(&oldValue, sizeof(oldValue));
	newValue.it_value = howMuchTimeFromNow(expiration);
	int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
	if(ret) {
		LOG_ERROR("timerfd_settime()");
	}
}


}
}
}

using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;

TimerQueue::TimerQueue(EventLoop *loop):
	_loop(loop),
	_timerfd(createTimerfd()),
	_timerfdChannel(loop, _timerfd),
	_timers()
{
	_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
	_timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
	// _timerfdChannel.disableAll();
	// _timerfdChannel.remove();
	::close(_timerfd);
	// do not remove channel, since we're in EventLoop::dtor();
}

void TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
	// std::unique_ptr<Timer> timer(new Timer(cb, when, interval));
	// _loop->assertInLoopThread();
	// bool earliestChanged = insert(timer);

	// if(earliestChanged) {
		// resetTimerfd(_timerfd, timer->expiration());
	// }
	// return TimerId(timer);
}

void TimerQueue::handleRead()
{
}
