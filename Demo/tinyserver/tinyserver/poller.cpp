#include <tinyserver/poller.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/sockets.h>

#include <assert.h>

using namespace tinyserver;

Poller::Poller(EventLoop *loop) :
	_loop(loop)
{
}

Poller::~Poller()
{
}

void Poller::poll(ChannelVector *activeChannels, int timeoutMs)
{
	assert(activeChannels != nullptr);

	int n = sockets::Poll(_pollfds.data(), _pollfds.size(), timeoutMs);
	if(n > 0) {
		for(auto pfd : _pollfds) {
			if(pfd.revents > 0) {
				auto channel = _channelMap[pfd.fd];
				channel->setRevents(pfd.revents);
				activeChannels->push_back(channel);
			}
		}
	} else if (n == 0) {
		LOG_TRACE("poll nothing happend");
	} else {
		LOG_ERROR("poll return error: " << n);
	}
}

void Poller::updateChannel(Channel *channel)
{
	assert(channel != nullptr);

	int fd = channel->fd();
	if(_channelMap.find(fd) == _channelMap.end()) {	// add a new channel
		struct pollfd pfd;
		pfd.fd = fd;
		pfd.events = channel->events();
		channel->setIndex(_pollfds.size());
		_pollfds.push_back(pfd);
		_channelMap[fd] = channel;
	} else {										// update an existed channel
		assert(channel->index() < _pollfds.size());
		auto& pfd = _pollfds.at(channel->index());
		assert(channel->fd() == pfd.fd);
		if(channel->isNoneEvent()) {
			pfd.events = -1;						// ignore the pollfd
		} else {
			pfd.events = channel->events();
		}
	}
}

void Poller::removeChannel(Channel *channel)
{
}
