#include <tinyserver/poller/EPollPoller.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <assert.h>
#include <sys/epoll.h>

using namespace tinyserver;

namespace
{
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
    _epollfd(::epoll_create1(EPOLL_CLOEXEC)),
    _events(kInitEventListSize)
{
    if(_epollfd < 0) {
        TLOG_FATAL("epoll_create1 error: " << errno);
    }
}

EPollPoller::~EPollPoller()
{
    sockets::Close(_epollfd);
}

void EPollPoller::poll(ChannelList *activeChannels, int timeoutMs)
{
	assert(activeChannels != nullptr);

    TLOG_TRACE("epoll fd count: " << _channelMap.size());
    int numEvents = ::epoll_wait(_epollfd, _events.data(), static_cast<int>(_events.size()), timeoutMs);
    int savedErrno = errno;
    if (numEvents > 0) {
        TLOG_TRACE(numEvents << " events happened");
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == _events.size()) {
            _events.resize(_events.size() * 2);
        }
    } else if (numEvents == 0) {
        TLOG_TRACE("epoll nothing happend");
	} else {
        if (savedErrno != EINTR) {
            TLOG_ERROR("epoll return error: " << savedErrno);
        }
	}
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= _events.size());
    for(int i=0; i<numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(_events[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        auto it = _channelMap.find(fd);
        assert(it != _channelMap.end());
        assert(it->second == channel);
#endif
        channel->setRevents(_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::updateChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    
    const int index = channel->index();
    TLOG_TRACE("update fd = " << channel->fd()
        << " events = " << channel->events()
        << " index = " << index);
    if (index == kNew || index == kDeleted) {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == kNew) {
            assert(_channelMap.find(fd) == _channelMap.end());
            _channelMap[fd] = channel;
        } else {    // index == kDelete
            assert(_channelMap.find(fd) != _channelMap.end());
            assert(_channelMap[fd] == channel);
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->fd();
        (void)fd;
        assert(_channelMap.find(fd) != _channelMap.end());
        assert(_channelMap[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel)
{
    Poller::assertInLoopThread();

    int fd = channel->fd();
    TLOG_TRACE("remove fd = " << fd);
    assert(_channelMap.find(fd) != _channelMap.end());
    assert(_channelMap[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = _channelMap.erase(fd);
    (void)n;
    assert(n == 1);

    if(index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EPollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.ptr = channel;
    event.events = channel->events();
    int fd = channel->fd();
    TLOG_TRACE("epoll_ctl op = " << operation << " fd = " << fd << " events = " << channel->events());
    if (::epoll_ctl(_epollfd, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            TLOG_ERROR("EPOLL_CTL_DEL error, fd = " << fd << " errno = " << errno);
        } else {
            TLOG_FATAL("epoll_ctl op = " << operation << " error, fd = " << fd << " errno = " << errno);
        }
    }
}