#include <muduo/net/channel.h>
#include <muduo/net/eventLoop.h>
#include <muduo/base/logger.h>
#include <poll.h>
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd):
	_loop(loop),
	_fd(fd),
	_events(0),
	_revents(0),
	_index(-1),
	_eventHandling(false)
{
}

Channel::~Channel()
{
	assert(!_eventHandling);
}

void Channel::handleEvent()
{
	_eventHandling = true;
	if(_revents & POLLNVAL) {
		LOG_WARN(__FUNCTION__ << " POLLNVAL");
	}

	if((_revents & POLLRDHUP) && !(_revents & POLLIN)) {
		LOG_WARN("Channel::handleEvent() POLLHUP");
		if(_closeCallback) {
			_closeCallback();
		}
	}
	
	if(_revents & (POLLERR | POLLNVAL)) {
		if(_errorCallback) {
			_errorCallback();
		}
	}
	if(_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
		if(_readCallback) {
			_readCallback();
		}
	}
	if(_revents & POLLOUT) {
		if(_writeCallback) {
			_writeCallback();
		}
	}
	_eventHandling = false;
}

void Channel::update()
{
	_loop->updateChannel(this);
}
