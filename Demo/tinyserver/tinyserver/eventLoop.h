#ifndef TINYSERVER_EVENTLOOP_H
#define TINYSERVER_EVENTLOOP_H

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <tinyserver/types.h>
#include <thread>
#include <vector>

namespace tinyserver
{

class Channel;
class Poller;
class TimerManager;

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

	/**
	 * @brief 启动一个定时器，在某一时间点调用回调函数
	 * 
	 * @param cb 回调函数
	 * @param time 系统时间点，必须大于当前时间
	 * @param interval 间隔时间，默认为0.0，如果值为大于0.0的数则每隔 @interval 秒重复调用 @cb 回调函数
	 * @return uint32_t 定时器ID
	 */
	uint32_t runAt(const TimerCallback& cb, std::chrono::system_clock::time_point time, double interval = 0.0);

	/**
	 * @brief 启动一个定时器，在一段时间后调用回调函数
	 * 
	 * @param cb 回调函数
	 * @param delay 延迟时间(秒数)，必须为大于0.0的值
	 * @param interval 间隔时间，默认为0.0，如果值为大于0.0的数则每隔 @interval 秒重复调用 @cb 回调函数
	 * @return uint32_t 定时器ID
	 */
	uint32_t runAfter(const TimerCallback& cb, double delay, double interval = 0.0);

	/**
	 * @brief 移除定时器
	 * 
	 * @param id 定时器ID
	 */
	void removeTimer(uint32_t id);

	/**
	 * @brief 移除所有定时器
	 * 
	 */
	void removeAllTimer();

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

	std::unique_ptr<TimerManager> _timerManager;

	void abortNotInLoopThread();

	void onWakeupChannelReading();
	void doPendingFunctors();
};

}

#endif // TINYSERVER_EVENTLOOP_H
