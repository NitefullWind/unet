#ifndef TINYSERVER_POLLER_H
#define TINYSERVER_POLLER_H

#include <map>
#include <vector>

struct pollfd;

namespace tinyserver
{

class Channel;
class EventLoop;

class Poller
{
public:
    explicit Poller(EventLoop *loop);
    virtual ~Poller();

    /**
     * @brief 开始轮询
     * 
     * @param activeChannels 轮询的Channel列表
     * @param timeoutMs 超时时间
     */
    virtual void poll(std::vector<Channel *> *activeChannels, int timeoutMs) = 0;
    
    /**
     * @brief 添加或更新一个Channel
     * 将Channel添加到轮询中，如果该Channel已经在轮询中，则对其更新
     * @param channel 要添加或更新的Channel
     */
    virtual void updateChannel(Channel *channel) = 0;

    /**
     * @brief 移除Channel
     * 从轮询中将Channel移除
     * @param channel 要删除的Channel
     */
    virtual void removeChannel(Channel *channel) = 0;

    /**
     * @brief 检查Channel是否在轮询中
     * 
     * @param channel 要检查的Channel
     * @return true 在轮询中
     * @return false 不走轮询中
     */
    bool hasChannel(Channel *channel) const;

    /**
     * @brief 确保在当前线程执行的断言
     */
    void assertInLoopThread() const;

private:
    EventLoop *_loop;

protected:
    std::map<int, Channel *> _channelMap;
};

}

#endif // TINYSERVER_POLLER_H
