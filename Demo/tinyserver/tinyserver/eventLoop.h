#ifndef TINYSERVER_EVENTLOOP_H
#define TINYSERVER_EVENTLOOP_H

#include <tinyserver/sockets.h>

#include <vector>

namespace tinyserver
{

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	void loop();
	void stop();

private:
	bool _looping;
	bool _stop;

	std::vector<int> _sockets;
	struct pollfd *_fds;
	unsigned long _nfds;
};

}

#endif // TINYSERVER_EVENTLOOP_H
