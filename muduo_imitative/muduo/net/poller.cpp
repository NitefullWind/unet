#include <muduo/net/poller.h>
#include <muduo/net/eventLoop.h>
#include <muduo/net/channel.h>
#include <muduo/base/logger.h>
#include <poll.h>
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

Poller::Poller(EventLoop* loop) :
	_ownerLoop(loop)
{
}

Poller::~Poller()
{
}

void Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = ::poll(&(*_pollfds.begin()), _pollfds.size(), timeoutMs);
	if(numEvents > 0) {
		LOG_TRACE(numEvents << " events happended");
		fillActiveChannels(numEvents, activeChannels);
	} else if(numEvents == 0) {
		LOG_TRACE("nothing happended");
	} else {
		LOG_ERROR(__FUNCTION__ << errno);
	}
}

void Poller::updateChannel(Channel* channel)
{
	assertInLoopThead();
	LOG_TRACE("fd = " << channel->fd() << " events = " << channel->events());

	if(channel->index() < 0) {		// a new one, add to _pollfds
		assert(_channels.find(channel->fd()) == _channels.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		_pollfds.push_back(pfd);
		int idx = static_cast<int>(_pollfds.size()) - 1;
		channel->setIndex(idx);
		_channels[pfd.fd] = channel;
	} else {							// update existing one
		assert(_channels.find(channel->fd()) != _channels.end());
		assert(_channels[channel->fd()] == channel);
		int idx = channel->index();
		assert(0 <= idx && idx <= static_cast<int>(_pollfds.size()));
		struct pollfd& pfd = _pollfds[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if(channel->isNoneEvent()) {	// ignore this pollfd
			pfd.fd = -channel->fd()-1;
		}
	}
}

void Poller::removeChannel(Channel *channel)
{
	assertInLoopThead();
	LOG_TRACE("fd = " << channel->fd());
	assert(_channels.find(channel->fd()) != _channels.end());
	assert(channel->isNoneEvent());
	int idx = channel->index();
	assert(0 <= idx && idx < static_cast<int>(_pollfds.size()));
	const struct pollfd& pfd = _pollfds[idx]; (void)pfd;
	assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
	size_t n = _channels.erase(channel->fd());
	assert(n == 1); (void)n;
	if((size_t)idx == _pollfds.size()-1) {
		_pollfds.pop_back();
	} else {
		int channelAtEnd = _pollfds.back().fd;
		std::iter_swap(_pollfds.begin()+idx, _pollfds.end()-1);
		if(channelAtEnd < 0) {
			channelAtEnd = -channelAtEnd-1;
		}
		_channels[channelAtEnd]->setIndex(idx);
		_pollfds.pop_back();
	}
}

void Poller::assertInLoopThead()
{
	_ownerLoop->assertInLoopThread();
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	for(PollFdList::const_iterator it=_pollfds.cbegin();
			it!=_pollfds.cend() && numEvents>0; ++it) {
		if(it->revents > 0) {
			--numEvents;
			auto ch_it = _channels.find(it->fd);
			assert(ch_it != _channels.end());
			auto channel = ch_it->second;
			assert(channel->fd() == it->fd);
			channel->setRevents(it->revents);
			activeChannels->push_back(channel);
		}
	}
}
