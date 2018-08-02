#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <thread>

namespace muduo
{
namespace net
{

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	void loop();

	void assertInLoopThread();

	bool isInLoopThread() const;
private:

	void abortNotInLoopThread();

	bool _looping;
	const std::thread::id _tid;
};

}
}

#endif // MUDUO_NET_EVENTLOOP_H
