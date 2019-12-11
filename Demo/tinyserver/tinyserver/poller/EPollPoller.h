#ifndef TINYSERVER_POLLER_EPollPoller_H
#define TINYSERVER_POLLER_EPollPoller_H

#include <tinyserver/poller.h>

#include <vector>

struct epoll_event;

namespace tinyserver
{

class Channel;
class EventLoop;

class EPollPoller : public Poller
{
public:
    explicit EPollPoller(EventLoop *loop);
    virtual ~EPollPoller();

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
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    void update(int operation, Channel* channel);

    typedef std::vector<struct epoll_event> EventList;
    
    int _epollfd;
    EventList _events;
};

}

#endif // TINYSERVER_POLLER_EPollPoller_H
