#include <muduo/base/logger.h>
#include <muduo/net/timerQueue.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/timerId.h>
#include <muduo/net/timer.h>
#include <sys/timerfd.h>
#include <strings.h>				// bzero()
#include <unistd.h>					// read()
#include <assert.h>

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
	_timers(),
	_callingExpiredTimers(false)
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
	for(TimerList::iterator it=_timers.begin();
			it!=_timers.end(); ++it) {
		delete it->second;
	}
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
	Timer *timer = new Timer(cb, when, interval);
	// 将所有的修改操作移到IO线程，确保addTimer线程安全
	_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
	_loop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer *timer)
{
	_loop->assertInLoopThread();
	bool earliestChanged = insert(timer);

	if(earliestChanged) {
		resetTimerfd(_timerfd, timer->expiration());
	}
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
	_loop->assertInLoopThread();
	assert(_timers.size() == _activeTimers.size());
	ActiveTimer timer(timerId._timer, timerId._sequence);
	ActiveTimerSet::iterator it = _activeTimers.find(timer);
	if(it != _activeTimers.end()) {
		size_t n = _timers.erase(Entry(it->first->expiration(), it->first));
		assert(n == 1); (void)n;
		delete it->first;
		_activeTimers.erase(it);
	} else if(_callingExpiredTimers) {
		_cancelingTimers.insert(timer);
	}
	assert(_timers.size() == _activeTimers.size());
}

void TimerQueue::handleRead()
{
	_loop->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(_timerfd, now);

	std::vector<Entry> expired = getExpired(now);

	_callingExpiredTimers = true;
	_cancelingTimers.clear();

	for(std::vector<Entry>::iterator it=expired.begin();
			it!=expired.end(); ++it) {
		it->second->run();
	}
	_callingExpiredTimers = false;

	reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	assert(_timers.size() == _activeTimers.size());
	std::vector<Entry> expired;
	Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	// 找到第一个过期时间晚于当前时间的(now < it->first)定时器的迭代器.
	// 此迭代器之前的元素都是已过期的，之后的元素都是未过期的
	TimerList::iterator it = _timers.lower_bound(sentry);
	assert(it == _timers.end() || now < it->first);
	std::copy(_timers.begin(), it, back_inserter(expired));
	_timers.erase(_timers.begin(), it);

	for(auto entry : expired) {
		ActiveTimer timer(entry.second, entry.second->sequence());
		size_t n = _activeTimers.erase(timer);
		assert(n == 1); (void)n;
	}

	assert(_timers.size() == _activeTimers.size());

	return expired;
}

void TimerQueue::reset(std::vector<Entry>& expired, Timestamp now)
{
	Timestamp nextExpire;
	for(std::vector<Entry>::iterator it = expired.begin();
			it!=expired.end(); ++it) {
		ActiveTimer timer(it->second, it->second->sequence());
		if(it->second->repeat()
				&& _cancelingTimers.find(timer) == _cancelingTimers.end()) {
			it->second->restart(now);
			insert(std::move(it->second));
		} else {
			// FIXME move to a free list
			delete it->second;
		}
	}
	if(!_timers.empty()) {
		nextExpire = _timers.begin()->second->expiration();
	}

	if(nextExpire.valid()) {
		resetTimerfd(_timerfd, nextExpire);
	}
}

bool TimerQueue::insert(Timer *timer)
{
	_loop->assertInLoopThread();
	assert(_timers.size() == _activeTimers.size());

	bool earliestChanged = false;
	Timestamp when = timer->expiration();
	TimerList::iterator it = _timers.begin();
	if(it==_timers.end() || when < it->first) {
		earliestChanged = true;
	}
	{
		// insert into _timers, items auto sorted by when
		std::pair<TimerList::iterator, bool> result = 
			_timers.insert(std::make_pair(when, timer));
		assert(result.second); (void)result;
	}
	{
		std::pair<ActiveTimerSet::iterator, bool> result
			= _activeTimers.insert(ActiveTimer(timer, timer->sequence()));
		assert(result.second); (void)result;
	}

	assert(_timers.size() == _activeTimers.size());
	return earliestChanged;
}
