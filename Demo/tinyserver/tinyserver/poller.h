#ifndef TINYSERVER_POLLER_H
#define TINYSERVER_POLLER_H

#include <map>
#include <vector>

struct pollfd;

namespace tinyserver
{

class Channel;
class EventLoop;

class Poller
{
public:
	explicit Poller(EventLoop *loop);
	~Poller();

	/*
	 * return active channel vector
	 */
	void poll(std::vector<Channel *> *activeChannels, int timeoutMs);
	
	/*
	 * add a new or update an existed channel
	 */
	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

private:
	EventLoop *_loop;
	std::vector<struct pollfd> _pollfds;
	std::map<int, Channel *> _channelMap;
};

}

#endif // TINYSERVER_POLLER_H
