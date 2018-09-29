#include <muduo/net/epoller.h>

#include <muduo/net/channel.h>
#include <muduo/base/logger.h>

#include <type_traits>		// static_assert()
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <strings.h>		// bzero()
#include <unistd.h>			// close()

using namespace muduo;
using namespace muduo::net;

static_assert(EPOLLIN == POLLIN, "static_assert EPOLLIN == POLLIN");
static_assert(EPOLLPRI == POLLPRI, "static_assert EPOLLPRI == POLLPRI");
static_assert(EPOLLOUT == POLLOUT, "static_assert EPOLLOUT == POLLOUT");
static_assert(EPOLLRDHUP == POLLRDHUP, "static_assert EPOLLRDHUP == POLLRDHUP");
static_assert(EPOLLERR == POLLERR, "static_assert EPOLLERR == POLLERR");
static_assert(EPOLLHUP == POLLHUP, "static_assert EPOLLHUP == POLLHUP");

namespace
{
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

EPoller::EPoller(EventLoop *loop):
	_ownerLoop(loop),
	_epollfd(::epoll_create1(EPOLL_CLOEXEC)),
	_events(kInitEventListSize)
{
	if(_epollfd < 0) {
		LOG_FATAL("EPoller::EPoller");
	}
}

EPoller::~EPoller()
{
	::close(_epollfd);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
	int numEvents = ::epoll_wait(_epollfd, 
			_events.data(), 
			static_cast<int>(_events.size()),
			timeoutMs);
	Timestamp now(Timestamp::now());
	if(numEvents > 0) {
		LOG_TRACE(numEvents << " events happended");
		fillActiveChannels(numEvents, activeChannels);
		if(static_cast<size_t>(numEvents) == _events.size()) {
			_events.resize(_events.size() * 2);
		}
	} else if (numEvents == 0) {
		LOG_TRACE("nothing happended");
	} else {
		LOG_FATAL("EPoller::poll()");
	}
	return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= _events.size());
	for(int i=0; i<numEvents; ++i) {
		Channel *channel = static_cast<Channel*>(_events[i].data.ptr);
#ifndef NDEBUG
		int fd = channel->fd();
		ChannelMap::const_iterator it = _channels.find(fd);
		assert(it != _channels.end());
		assert(it->second == channel);
#endif
		channel->setRevents(_events[i].events);
		activeChannels->push_back(channel);
	}
}

void EPoller::updateChannel(Channel *channel)
{
	assertInLoopThread();
	LOG_TRACE("fd = " << channel->fd() << " events = " << channel->events());
	const int index = channel->index();
	if(index == kNew || index == kDeleted) {
		// a new one, add with EPOLL_CTL_ADD
		int fd = channel->fd();
		if(index == kNew) {
			assert(_channels.find(fd) == _channels.end());
			_channels[fd] = channel;
		} else {
			assert(_channels.find(fd) != _channels.end());
			assert(_channels[fd] == channel);
		}
		channel->setIndex(kAdded);
		update(EPOLL_CTL_ADD, channel);
	} else {
		// update existing one with EPOLL_CTL_MOD/DEL
		int fd = channel->fd();
		(void)fd;
		assert(_channels.find(fd) != _channels.end());
		assert(_channels[fd] == channel);
		assert(index == kAdded);
		if(channel->isNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->setIndex(kDeleted);
		} else {
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void EPoller::removeChannel(Channel *channel)
{
	assertInLoopThread();
	int fd = channel->fd();
	LOG_TRACE("fd = " << fd);
	assert(_channels.find(fd) != _channels.end());
	assert(_channels[fd] == channel);
	assert(channel->isNoneEvent());
	int index = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n = _channels.erase(fd);
	(void)n;
	assert(n == 1);

	if(index == kAdded)
	{
		update(EPOLL_CTL_DEL, channel);
	}
	channel->setIndex(kNew);
}

void EPoller::update(int operation, Channel *channel)
{
	struct epoll_event event;
	bzero(&event, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();
	if(::epoll_ctl(_epollfd, operation, fd, &event) < 0) {
		LOG_FATAL("epoll_ctl op = " << operation << " fd = " << fd);
	}
}

