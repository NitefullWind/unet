#ifndef TINYSERVER_EVENTLOOP_H
#define TINYSERVER_EVENTLOOP_H

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace tinyserver
{

class Channel;
class Poller;

class EventLoop
{
public:
	typedef std::function<void()> Functor;

	EventLoop();
	~EventLoop();

	void loop();
	void stop();

	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

	void runInLoop(const Functor& cb);
	void queueInLoop(const Functor& cb);

	/*
	 * wake up from poll
	 */
	void wakeUp();

	void assertInLoopThread();
	bool isInLoopThread() const { return _threadId == std::this_thread::get_id(); }
private:
	bool _looping;
	bool _stop;
	std::thread::id _threadId;
	std::mutex _mutex;

	std::unique_ptr<Poller> _poller;

	std::unique_ptr<Channel> _wakeupChannel;

	std::vector<Functor> _pendingFunctors;
	bool _callingPendingFunctors;

	void abortNotInLoopThread();

	void onWakeupChannelReading();
	void doPendingFunctors();
};

}

#endif // TINYSERVER_EVENTLOOP_H
