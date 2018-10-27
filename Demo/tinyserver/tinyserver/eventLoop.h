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

	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

	/*
	 * wake up from poll
	 */
	void wakeUp();

private:
	bool _looping;
	bool _stop;

	std::unique_ptr<Poller> _poller;

	std::unique_ptr<Channel> _wakeupChannel;

	void onWakeupChannelReading();
};

}

#endif // TINYSERVER_EVENTLOOP_H
