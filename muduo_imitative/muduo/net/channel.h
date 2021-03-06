#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include <muduo/base/timestamp.h>

#include <functional>

namespace muduo
{
namespace net
{

class EventLoop;

class Channel
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;

	Channel(EventLoop *loop, int fd);
	~Channel();

	void handleEvent(Timestamp receiveTime);
	void setReadCallback(const ReadEventCallback& cb) { this->_readCallback = cb; }
	void setWriteCallback(const EventCallback& cb) { this->_writeCallback = cb; }
	void setErrorCallback(const EventCallback& cb) { this->_errorCallback = cb; }
	void setCloseCallback(const EventCallback& cb) { this->_closeCallback = cb; }

	int fd() const { return _fd; }
	int events() const { return _events; }
	void setRevents(int revt) { this->_revents = revt; }
	bool isNoneEvent() const { return this->_events == kNoneEvent; }

	void enableReading() { _events |= kReadEvent; update(); }
	void enableWriting() { _events |= kWriteEvent; update(); }
	void disableWriting() { _events &= ~kWriteEvent; update(); }
	void disableAll() { _events &= kNoneEvent; update(); }
	bool isWriting() const { return _events & kWriteEvent; }
	
	// for Poller
	int index() { return _index; }
	void setIndex(int idx) { this->_index = idx; }

	EventLoop *ownerLoop() { return _loop; }

private:
	void update();

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop *_loop;
	const int _fd;
	int _events;
	int _revents;
	int _index;			// used by poller
	bool _eventHandling;

	ReadEventCallback _readCallback;
	EventCallback _writeCallback;
	EventCallback _errorCallback;
	EventCallback _closeCallback;
};

}
}

#endif // MUDUO_NET_CHANNEL_H

