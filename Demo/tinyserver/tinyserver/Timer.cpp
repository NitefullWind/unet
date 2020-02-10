#include <tinyserver/Timer.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>
#include <tinyserver/TimerManager.h>

#include <assert.h>
#include <chrono>
#include <ctime>
#include <cmath>
#include <sys/timerfd.h>

using namespace tinyserver;

namespace
{
int createTimerFd()
{
    int fd = ::timerfd_create(CLOCK_MONOTONIC,
                            TFD_CLOEXEC | TFD_NONBLOCK);
    if (fd < 0) {
        TLOG_FATAL("timerfd create error.")
    }
    return fd;
}

inline time_t seconds2Nanoseconds(double seconds)
{
    return (time_t)(seconds * 1000000000);
}
}

Timer::Timer(TimerManager* tm,
                uint32_t id,
                const TimerCallback& cb, 
                double seconds, 
                double interval)
    : _tm(tm),
      _loop(tm->eventLoop()),
      _id(id),
      _fd(createTimerFd()),
      _timerChannel(_loop, _fd),
      _timerCallback(cb),
      _seconds(seconds),
      _interval(interval)
{
    _timerChannel.setReadCallback(std::bind(&Timer::onTimerChannelRead, this));
    _timerChannel.enableReading();
    _loop->updateChannel(&_timerChannel);

    struct itimerspec newValue;
    bzero(&newValue, sizeof(newValue));
    newValue.it_value.tv_sec = (time_t)_seconds;
    newValue.it_value.tv_nsec = seconds2Nanoseconds(_seconds - _seconds);
    newValue.it_interval.tv_sec = (time_t)_interval;
    newValue.it_interval.tv_nsec = seconds2Nanoseconds(_interval - _interval);
    int ret = ::timerfd_settime(_fd, 0, &newValue, nullptr);
    if (ret == -1) {
        TLOG_ERROR("addTimer() timerfd_settime error.");
    }
}

Timer::~Timer()
{
    _timerChannel.disableAll();
    _loop->removeChannel(&_timerChannel);
    sockets::Close(_fd);
}

void Timer::onTimerChannelRead()
{
    int fd = _timerChannel.fd();

    auto now = std::chrono::system_clock::now();
    auto nowt = std::chrono::system_clock::to_time_t(now);
    uint64_t timeoutTimes = 0;
    ssize_t n = sockets::Read(fd, &timeoutTimes, sizeof(timeoutTimes));
    TLOG_TRACE("onTimerChannelRead() " << timeoutTimes << " at " << std::ctime(&nowt));
    if (n != sizeof(timeoutTimes)) {
        TLOG_ERROR("onTimerChannelRead() reads " << n << " bytes instead of " << sizeof(timeoutTimes));
    }
        
    for (size_t i = 0; i < timeoutTimes; i++)
    {
        if(_timerCallback) {
            _timerCallback();
        }
    }
    if (_interval <= 0.0) {
        _tm->removeTimer(_id);
    }
}