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
	_loop->assertInLoopThread();
	assert(channel != nullptr);

	int fd = channel->fd();
	if(_channelMap.find(fd) == _channelMap.end()) {	// add a new channel
		struct pollfd pfd;
		pfd.fd = fd;
		pfd.events = channel->events();
		pfd.revents = 0;
		channel->setIndex(_pollfds.size());
		_pollfds.push_back(pfd);
		_channelMap[fd] = channel;
	} else {	
		assert(channel->index() < _pollfds.size());
		auto& pfd = _pollfds.at(channel->index());
		assert(channel->fd() == pfd.fd);
		if(channel->isNoneEvent()) {
			pfd.fd = -pfd.fd-1;						// ignore the pollfd
		} else {
			pfd.events = channel->events();
		}
		pfd.revents = 0;
	}

	LOG_TRACE("========update pollfds=======");
	for(auto pfd : _pollfds) {
		LOG_TRACE("**[fd]=" << pfd.fd << "[events]=" << pfd.events << "[revents]=" << pfd.revents);
	}
}

void Poller::removeChannel(Channel *channel)
{
	_loop->assertInLoopThread();
	assert(channel != nullptr);
	channel->disableAll();

	size_t index = channel->index();
	auto chit = _channelMap.find(channel->fd());
	
	assert(_pollfds.size() > index);
	assert(chit != _channelMap.end());


	// remove from pollfds
	if(_pollfds.size()-1 == index) {
		_pollfds.pop_back();
	} else {
		int lastPollfd = _pollfds.back().fd;
		std::iter_swap(_pollfds.begin()+index, _pollfds.end()-1);	// swap with the last item to avoid reset other channel's index
		_pollfds.pop_back();										// remove new last channel
		_channelMap[lastPollfd]->setIndex(index);					// update old last channel's index
	}
	_channelMap.erase(chit);										// remove from map

	LOG_TRACE("========remove pollfds=======");
	for(auto pfd : _pollfds) {
		LOG_TRACE("**[fd]=" << pfd.fd << "[events]=" << pfd.events << "[revents]=" << pfd.revents);
	}
}
