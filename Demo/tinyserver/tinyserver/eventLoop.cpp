#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>

#define OPEN_MAX 1024

using namespace tinyserver;

EventLoop::EventLoop() :
	_looping(false),
	_stop(false),
	_fds(new pollfd[OPEN_MAX]),
	_nfds(0)
{
}

EventLoop::~EventLoop()
{
	delete _fds;
}

void EventLoop::loop()
{
	_looping = true;

	for(int i=0; i<static_cast<int>(_sockets.size()); ++i) {
		if(i >= OPEN_MAX) {
			LOG_FATAL("need more fds");
		}
		_fds[i].fd = _sockets[i];
		_fds[i].events = POLLRDNORM;
		_nfds++;
	}

	while(!_stop) {
		int n = sockets::Poll(_fds, _nfds, 10 * 1000);
		LOG_TRACE("poll return: " << n);
		//! for test
		_stop = true;
	}

	_looping = false;
}

void EventLoop::stop()
{
	_stop = true;
}
