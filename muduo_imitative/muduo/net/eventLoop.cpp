#include <muduo/net/eventLoop.h>
#include <muduo/net/channel.h>
#include <muduo/net/poller.h>
#include <muduo/net/timerQueue.h>
#include <muduo/base/logger.h>
#include <assert.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

static int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(evtfd < 0) {
		LOG_FATAL("Failed in eventfd");
		abort();
	}
	return evtfd;
}

EventLoop::EventLoop() :
	_looping(false),
	_quit(false),
	_tid(std::this_thread::get_id()),
	_poller(std::unique_ptr<Poller>(new Poller(this))),
	_timerQueue(new TimerQueue(this)),
	_wakeupFd(createEventfd()),
	_wakeupChannel(new Channel(this, _wakeupFd))
{
	LOG_TRACE("EventLoop created " << this << " in thread " << _tid);
	if(t_loopInThisThread) {
		LOG_FATAL("Another EventLoop " << t_loopInThisThread
				<< " exists in this thread " << _tid);
	} else {
		t_loopInThisThread = this;
	}
	_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
	_wakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
	assert(!_looping);
	::close(_wakeupFd);
	t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
	assert(!_looping);
	assertInLoopThread();
	_looping = true;
	_quit = false;

	while(!_quit) {
		_activeChannels.clear();
		_poller->poll(kPollTimeMs, &_activeChannels);
		for(ChannelList::iterator it = _activeChannels.begin();
				it!=_activeChannels.end(); ++it) {
			(*it)->handleEvent();
		}
		doPendingFunctors();
	}

	LOG_TRACE("EventLoop " << this << " stop looping");
	_looping = false;
}

void EventLoop::quit()
{
	_quit = true;
	if(!isInLoopThread()) {
		wakeup();
	}
}

void EventLoop::runInLoop(const Functor& cb)
{
	if(isInLoopThread()) {
		cb();
	} else {
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		std::lock_guard<std::mutex> guard(_mutex);
		_pendingFunctors.push_back(cb);
	}

	if(!isInLoopThread() || _callingPendingFunctors) {
		wakeup();
	}
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
	return _timerQueue->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	return _timerQueue->addTimer(cb, time, interval);
}

void EventLoop::updateChannel(Channel *channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	_poller->updateChannel(channel);
}

void EventLoop::assertInLoopThread()
{
	if(!isInLoopThread()) {
		abortNotInLoopThread();
	}
}

inline bool EventLoop::isInLoopThread() const
{
	return (_tid == std::this_thread::get_id());
}

void EventLoop::abortNotInLoopThread()
{
	LOG_FATAL(__FUNCTION__ << " EventLoop " << this
			<< " was created in thread id = " << _tid
			<< ", current thread id = " << std::this_thread::get_id());
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(_wakeupFd, &one, sizeof(one));
	if(n != sizeof(one)) {
		LOG_ERROR("EventLoop::wakeup() writes " << " bytes instead of 8");
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(_wakeupFd, &one, sizeof(one));
	if(n != sizeof(one)) {
		LOG_ERROR("EventLoop::handleRead() reads " << n << " bytes instead of 8")
	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	_callingPendingFunctors = true;

	{
		std::lock_guard<std::mutex> gurad(_mutex);
		functors.swap(_pendingFunctors);
	}

	for(size_t i=0; i<functors.size(); ++i) {
		functors[i]();
	}
	_callingPendingFunctors = false;
}
