#ifndef TINYSERVER_EVENTLOOP_H
#define TINYSERVER_EVENTLOOP_H

#include <memory>

namespace tinyserver
{

class Channel;
class Poller;

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	void loop();
	void stop();

	void addChannel(Channel *channel);
	void updateChannel(Channel *channel);

private:
	bool _looping;
	bool _stop;

	std::unique_ptr<Poller> _poller;
};

}

#endif // TINYSERVER_EVENTLOOP_H
