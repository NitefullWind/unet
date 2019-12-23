#ifndef TINYSERVER_TIMER_H
#define TINYSERVER_TIMER_H

#include <tinyserver/channel.h>
#include <tinyserver/types.h>

namespace tinyserver
{

class EventLoop;
class TimerManager;

/**
 * @brief 定时器类
 * 
 */
class Timer
{
public:
    explicit Timer(TimerManager* tm, 
                    uint32_t id,
                    const TimerCallback& cb, 
                    double seconds, 
                    double interval);
    ~Timer();

    uint32_t id() const { return _id; }
    int fd() const { return _fd; }
    const TimerCallback& timerCallback() const { return _timerCallback; }

private:
    TimerManager* _tm;
    EventLoop* _loop;
    uint32_t _id;
    int _fd;
    Channel _timerChannel;
    TimerCallback _timerCallback;
    double _seconds;
    double _interval;

    void onTimerChannelRead();
};

} // namespace tinyserver


#endif // TINYSERVER_TIMER_H