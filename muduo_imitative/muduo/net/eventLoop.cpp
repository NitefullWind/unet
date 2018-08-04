#include <muduo/net/eventLoop.h>
#include <muduo/net/channel.h>
#include <muduo/net/poller.h>
#include <muduo/net/timerQueue.h>
#include <muduo/base/logger.h>
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

EventLoop::EventLoop() :
	_looping(false),
	_quit(false),
	_tid(std::this_thread::get_id()),
	_poller(std::unique_ptr<Poller>(new Poller(this))),
	_timerQueue(std::unique_ptr<TimerQueue>(new TimerQueue(this)))
{
	LOG_TRACE("EventLoop created " << this << " in thread " << _tid);
	if(t_loopInThisThread) {
		LOG_FATAL("Another EventLoop " << t_loopInThisThread
				<< " exists in this thread " << _tid);
	} else {
		t_loopInThisThread = this;
	}
}

EventLoop::~EventLoop()
{
	assert(!_looping);
	t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
	assert(!_looping);
	assertInLoopThread();
	_looping = true;

	while(!_quit) {
		_activeChannels.clear();
		_poller->poll(kPollTimeMs, &_activeChannels);
		for(ChannelList::iterator it = _activeChannels.begin();
				it!=_activeChannels.end(); ++it) {
			(*it)->handleEvent();
		}
	}

	LOG_TRACE("EventLoop " << this << " stop looping");
	_looping = false;
}

void EventLoop::quit()
{
	_quit = true;
}

void EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
	_timerQueue->addTimer(cb, time, 0.0);
}

void EventLoop::runAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

void EventLoop::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	_timerQueue->addTimer(cb, time, interval);
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
