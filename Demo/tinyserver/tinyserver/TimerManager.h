#ifndef TINYSERVER_TIMERMANAGER_H
#define TINYSERVER_TIMERMANAGER_H

#include <map>
#include <tinyserver/types.h>
#include <atomic>

namespace tinyserver
{

class EventLoop;
class Timer;

/**
 * @brief 定时器管理类
 * 
 */
class TimerManager
{
public:
    explicit TimerManager(EventLoop* loop);
    ~TimerManager();

    EventLoop* eventLoop() const { return _loop; }

    /**
     * @brief 增加定时器
     * 可以在其他线程调用
     * @param cb 到期后执行的回调函数
     * @param sec 到期时间(秒数)，必须为大于0.0的值
     * @param interval 间隔时间，默认为0.0，如果值为大于0.0的数则每隔 @interval 秒重复调用 @cb 回调函数
     * @return uint32_t 定时器ID
     */
    uint32_t addTimer(const TimerCallback& cb, double sec, double interval = 0.0);

    /**
     * @brief 移除定时器
     * 可以在其他线程调用
     * @param id 定时器ID
     */
    void removeTimer(uint32_t id);

    /**
     * @brief 移除所有定时器
     * 可以在其他线程调用
     */
    void removeAll();

private:
    EventLoop* _loop;
    std::atomic_uint_least32_t _timerIndex;
    std::map<uint32_t, Timer*> _timerMap;
    std::map<int, uint32_t> _timerFdMap;

    void addTimerInLoop(uint32_t timerId, const TimerCallback& cb, double sec, double interval = 0.0);
    void removeTimerInLoop(uint32_t id);
    void removeAllInLoop();
};
} // namespace tinyserver

#endif // TINYSERVER_TIMERQUEUE_H