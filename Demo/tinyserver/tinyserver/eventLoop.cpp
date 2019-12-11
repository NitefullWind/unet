#include <tinyserver/eventLoop.h>
#include <tinyserver/channel.h>
#include <tinyserver/logger.h>
#include <tinyserver/poller/EPollPoller.h>
#include <tinyserver/sockets.h>

#include <assert.h>
// #include <signal.h>
#include <sys/eventfd.h>

using namespace tinyserver;

namespace
{

int createEventFd()
{
	int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(fd < 0) {
		TLOG_FATAL(__FUNCTION__);
		abort();
	}
	return fd;
}

// #pragma GCC diagnostic ignored "-Wold-style-cast"
// class IgnoreSigPipe
// {
// public:
	// IgnoreSigPipe()
	// {
		// ::signal(SIGPIPE, SIG_IGN);
	// }
// };
// #pragma GCC diagnostic error "-Wold-style-cast"

}

EventLoop::EventLoop() :
	_looping(false),
	_stop(false),
	_threadId(std::this_thread::get_id()),
	_poller(new EPollPoller(this)),
	_wakeupChannel(new Channel(this, createEventFd())),
	_callingPendingFunctors(false)
{
	_wakeupChannel->setReadCallback(std::bind(&EventLoop::onWakeupChannelReading, this));
	_wakeupChannel->enableReading();
	updateChannel(_wakeupChannel.get());
}

EventLoop::~EventLoop()
{
	TLOG_TRACE(__FUNCTION__);
	_wakeupChannel->disableAll();
	sockets::Close(_wakeupChannel->fd());
	removeChannel(_wakeupChannel.get());
}

void EventLoop::loop()
{
	assert(_looping == false);
	_looping = true;

	while(!_stop) {
		std::vector<Channel*> activeChannels;
		_poller->poll(&activeChannels, 60 * 1000);
		for(auto channel : activeChannels) {
			channel->handleEvent();			// check events and call the callback function
		}
		doPendingFunctors();
	}

	_looping = false;
}

void EventLoop::stop()
{
	_stop = true;
}

void EventLoop::updateChannel(Channel *channel)
{
	// TLOG_TRACE(__FUNCTION__);
	_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
	_poller->removeChannel(channel);
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
		std::lock_guard<std::mutex> lk(_mutex);
		_pendingFunctors.push_back(cb);
	}
	if(!isInLoopThread() || _callingPendingFunctors) {
		wakeUp();
	}
}

void EventLoop::wakeUp()
{
	uint64_t one = 1;
	sockets::Write(_wakeupChannel->fd(), &one, sizeof(one));
}

void EventLoop::assertInLoopThread()
{
	if(!isInLoopThread()) {
		abortNotInLoopThread();
	}
}

void EventLoop::abortNotInLoopThread()
{
	TLOG_FATAL(__FUNCTION__ << " EventLoop " << this
			<< " was created in thread id = " << _threadId
			<< ", current thread id = " << std::this_thread::get_id());
	abort();
}

void EventLoop::onWakeupChannelReading()
{
	uint64_t one = 1;
	sockets::Read(_wakeupChannel->fd(), &one, sizeof(one));
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	_callingPendingFunctors = true;

	{
		std::lock_guard<std::mutex> lk(_mutex);
		functors.swap(_pendingFunctors);
	}

	for(auto func : functors) {
		func();
	}
	_callingPendingFunctors = false;
}
