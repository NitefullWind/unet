#include <tinyserver/poller.h>
#include <tinyserver/channel.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/poller/PollPoller.h>
#include <tinyserver/poller/EPollPoller.h>

#include <stdlib.h>

using namespace tinyserver;

Poller::Poller(EventLoop *loop) :
    _loop(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(Channel *channel) const
{
    assertInLoopThread();
    
    std::map<int, Channel *>::const_iterator it = _channelMap.find(channel->fd());
    return (it != _channelMap.end() && it->second == channel);
}

void Poller::assertInLoopThread() const
{
    _loop->assertInLoopThread();
}

std::unique_ptr<Poller> Poller::getNewPoller(EventLoop* loop)
{
    if (::getenv("TINYSERVER_USE_POLL")) {
        return std::unique_ptr<Poller>(new PollPoller(loop));
    } else {
        return std::unique_ptr<Poller>(new EPollPoller(loop));
    }
}