#ifndef TINYSERVER_EVENTLOOP_H
#define TINYSERVER_EVENTLOOP_H

namespace tinyserver
{

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	bool loop();
};

}

#endif // TINYSERVER_EVENTLOOP_H
