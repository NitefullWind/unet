#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <thread>
#include <vector>
#include <memory>

namespace muduo
{
namespace net
{

class Channel;
class Poller;

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	void loop();

	void quit();

	void updateChannel(Channel *channel);

	void assertInLoopThread();

	bool isInLoopThread() const;
private:
	void abortNotInLoopThread();

	typedef std::vector<Channel *> ChannelList;

	bool _looping;
	bool _quit;
	const std::thread::id _tid;
	std::unique_ptr<Poller> _poller;
	ChannelList _activeChannels;
};

}
}

#endif // MUDUO_NET_EVENTLOOP_H
