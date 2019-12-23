#include <tinyserver/TimerManager.h>
#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>
#include <tinyserver/Timer.h>

using namespace tinyserver;

TimerManager::TimerManager(EventLoop* loop)
    : _loop(loop),
      _timerIndex(0)
{
}

TimerManager::~TimerManager()
{
    removeAll();
}

uint32_t TimerManager::addTimer(const TimerCallback& cb, double sec, double interval)
{
    _timerIndex++;
    uint32_t timerId = _timerIndex.load();

    _loop->runInLoop(std::bind(&TimerManager::addTimerInLoop, this, 
                                timerId, cb, sec, interval));

    return timerId;
}

void TimerManager::removeTimer(uint32_t id)
{
    _loop->runInLoop(std::bind(&TimerManager::removeTimerInLoop, this, id));
}

void TimerManager::removeAll()
{
    _loop->runInLoop(std::bind(&TimerManager::removeAllInLoop, this));
}

void TimerManager::addTimerInLoop(uint32_t timerId, const TimerCallback& cb, double sec, double interval)
{
    _loop->assertInLoopThread();

    Timer* timer = new Timer(this, timerId, cb, sec, interval);    // 指针在移除定时器时delete
    _timerMap[timerId] = timer;
    _timerFdMap[timer->fd()] = timerId;
    TLOG_DEBUG("add timer id = " << timerId << " fd = " << timer->fd() << " sec = " << sec << " interval = " << interval);
}

void TimerManager::removeTimerInLoop(uint32_t id)
{
    _loop->assertInLoopThread();

    if (_timerMap.find(id) != _timerMap.end()) {
        auto timer = _timerMap[id];
        if (timer) {
            if (_timerFdMap.find(timer->fd()) != _timerFdMap.end()) {
                _timerFdMap.erase(timer->fd());
            }
            // 删除timer指针
            delete timer;
            timer = nullptr;
        }
        _timerMap.erase(id);
        TLOG_DEBUG("remove timer id: " << id << " success.")
    }
}

void TimerManager::removeAllInLoop()
{
    _loop->assertInLoopThread();

    for (auto &&it = _timerMap.begin(); it != _timerMap.end(); it++)
    {
        removeTimerInLoop(it->first);
    }
}