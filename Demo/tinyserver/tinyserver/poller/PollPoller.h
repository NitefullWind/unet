#ifndef TINYSERVER_POLLER_POLLPOLLER_H
#define TINYSERVER_POLLER_POLLPOLLER_H

#include <tinyserver/poller.h>

struct pollfd;

namespace tinyserver
{

class Channel;
class EventLoop;

class PollPoller : public Poller
{
public:
    explicit PollPoller(EventLoop *loop);
    virtual ~PollPoller();

    /*
     * @override
     */
    virtual void poll(ChannelList *activeChannels, int timeoutMs);
    
    /*
     * @override
     */
    virtual void updateChannel(Channel *channel);

    /*
     * @override
     */
    virtual void removeChannel(Channel *channel);

private:
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList _pollfds;
};

}

#endif // TINYSERVER_POLLER_POLLPOLLER_H
