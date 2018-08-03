#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

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

	Channel(EventLoop *loop, int fd);
	~Channel();

	void handleEvent();
	void setReadCallback(const EventCallback& cb) { this->_readCallback = cb; }
	void setWriteCallback(const EventCallback& cb) { this->_writeCallback = cb; }
	void setErrorCallback(const EventCallback& cb) { this->_errorCallback = cb; }

	int fd() const { return _fd; }
	int events() const { return _events; }
	void setRevents(int revt) { this->_revents = revt; }
	bool isNoneEvent() const { return this->_events == kNoneEvent; }

	void enableReading() { _events |= kReadEvent; update(); }
	// void enableWriting() { _events |= kWriteEvent; update(); }
	// void disableWriting() { _events &= ~kWriteEvent; update(); }
	// void disable() { _events &= kNoneEvent; update(); }
	
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

	EventCallback _readCallback;
	EventCallback _writeCallback;
	EventCallback _errorCallback;
};

}
}

#endif // MUDUO_NET_CHANNEL_H

