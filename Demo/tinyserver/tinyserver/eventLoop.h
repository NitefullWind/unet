#ifndef TINYSERVER_EVENTLOOP_H
#define TINYSERVER_EVENTLOOP_H

#include <tinyserver/sockets.h>

#include <vector>
#include <set>

namespace tinyserver
{

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	void loop();
	void stop();

	void addSockets(int sockfd);

private:
	bool _looping;
	bool _stop;

	std::vector<int> _sockets;
	std::vector<struct pollfd> _fds;
	std::set<int> _fdset;
};

}

#endif // TINYSERVER_EVENTLOOP_H
