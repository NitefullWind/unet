#include <tinyserver/eventLoop.h>
#include <tinyserver/channel.h>
#include <tinyserver/logger.h>
#include <tinyserver/poller.h>

using namespace tinyserver;

EventLoop::EventLoop() :
	_looping(false),
	_stop(false),
	_poller(new Poller(this))
{
}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
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
	_poller->updateChannel(channel);
}
