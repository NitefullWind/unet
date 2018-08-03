#include <muduo/net/channel.h>
#include <muduo/net/eventLoop.h>
#include <muduo/base/logger.h>
#include <poll.h>

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
	_index(-1)
{
}

Channel::~Channel()
{
}

void Channel::handleEvent()
{
	if(_revents & POLLNVAL) {
		LOG_WARN(__FUNCTION__ << " POLLNVAL");
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
}

void Channel::update()
{
	_loop->updateChannel(this);
}
