#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

using namespace tinyserver;

const short Channel::kNoneEvent = 0;
const short Channel::kReadEvent = POLLIN | POLLPRI;
const short Channel::kWriteEvent = POLLOUT;

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
	LOG_TRACE(__FUNCTION__ << " index:" << _index);
}

void Channel::update()
{
	_loop->assertInLoopThread();

	_loop->updateChannel(this);
}

void Channel::handleEvent()
{
	_loop->assertInLoopThread();

	if(_revents & POLLNVAL) {
		LOG_WARN(__FUNCTION__ << " POLLNVAL");
	}

	// Stream socket peer closed connection, or shut down writing half of connection.
	if((_revents & POLLRDHUP) && !(_revents & POLLIN)) {
		LOG_WARN(__FUNCTION__ << " POLLHUP");
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
}
