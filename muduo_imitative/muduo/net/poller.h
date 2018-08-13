#ifndef MUDUO_NET_POLLER_H
#define MUDUO_NET_POLLER_H

#include <vector>
#include <map>

struct pollfd;

namespace muduo
{
namespace net
{

class EventLoop;
class Channel;

class Poller
{
public:
	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop *loop);
	~Poller();

	void poll(int timeoutMs, ChannelList* activeChannels);

	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);

	void assertInLoopThead();

private:
	void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int, Channel*> ChannelMap;

	EventLoop* _ownerLoop;
	PollFdList _pollfds;
	ChannelMap _channels;
};

}
}

#endif // MUDUO_NET_POLLER_H
