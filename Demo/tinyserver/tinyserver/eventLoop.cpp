#include <tinyserver/eventLoop.h>
#include <tinyserver/channel.h>
#include <tinyserver/logger.h>
#include <tinyserver/poller.h>
#include <tinyserver/sockets.h>

#include <assert.h>
#include <sys/eventfd.h>

using namespace tinyserver;

namespace
{

int createEventFd()
{
	int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(fd < 0) {
		LOG_FATAL(__FUNCTION__);
		abort();
	}
	return fd;
}

}

EventLoop::EventLoop() :
	_looping(false),
	_stop(false),
	_poller(new Poller(this)),
	_wakeupChannel(new Channel(this, createEventFd()))
{
	_wakeupChannel->setReadCallback(std::bind(&EventLoop::onWakeupChannelReading, this));
	_wakeupChannel->enableReading();
	updateChannel(_wakeupChannel.get());
}

EventLoop::~EventLoop()
{
	LOG_TRACE(__FUNCTION__);
	_wakeupChannel->disableAll();
	sockets::Close(_wakeupChannel->fd());
	removeChannel(_wakeupChannel.get());
}

void EventLoop::loop()
{
	assert(_looping == false);
	_looping = true;

	while(!_stop) {
		ChannelVector activeChannels;
		_poller->poll(&activeChannels, 60 * 1000);
		for(auto channel : activeChannels) {
			channel->handleEvent();			// check events and call the callback function
		}
	}

	_looping = false;
}

void EventLoop::stop()
{
	_stop = true;
}

void EventLoop::updateChannel(Channel *channel)
{
	LOG_TRACE(__FUNCTION__);
	_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
	_poller->removeChannel(channel);
}

void EventLoop::wakeUp()
{
	uint64_t one = 1;
	sockets::Write(_wakeupChannel->fd(), &one, sizeof(one));
}

void EventLoop::onWakeupChannelReading()
{
	uint64_t one = 1;
	sockets::Read(_wakeupChannel->fd(), &one, sizeof(one));
}
