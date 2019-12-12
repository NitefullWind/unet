#ifndef TINYSERVER_CHANNEL_H
#define TINYSERVER_CHANNEL_H

#include <functional>

namespace tinyserver
{

class EventLoop;

class Channel
{
	typedef std::function<void()> EventCallback;

public:
	explicit Channel(EventLoop *loop, int fd);
	~Channel();

	int fd() const { return _fd; }

	uint32_t events() const { return  _events; }
	uint32_t revents() const { return _revents; }
	void setRevents(uint32_t revents) { _revents = revents; }

	int index() const { return _index; }
	void setIndex(int index) { _index = index; }

	void handleEvent();

	void enableReading() { if(!isReading()) {_events |= kReadEvent; update();} }
	void enableWriting() { if(!isWriting()) {_events |= kWriteEvent; update();} }
	void disableWriting() { if(isWriting()) {_events &= (uint32_t)~kWriteEvent; update();} }
	void disableAll() { if(!isNoneEvent()) {_events &= kNoneEvent; update();} }
	bool isReading() const { return _events & kReadEvent; }
	bool isWriting() const { return _events & kWriteEvent; }
	bool isNoneEvent() const { return (_events == kNoneEvent); }

	void setCloseCallback(const EventCallback& cb) { _closeCallback = cb; }
	void setErrorCallback(const EventCallback& cb) { _errorCallback = cb; }
	void setReadCallback(const EventCallback& cb) { _readCallback = cb; }
	void setWriteCallback(const EventCallback& cb) { _writeCallback = cb; }
private:
	EventLoop *_loop;
	const int _fd;
	uint32_t _events;		// struct pollfd.events
	uint32_t _revents;		// struct pollfd.revents
	int _index;		// for pollfds in poller

	static const uint32_t kNoneEvent;
	static const uint32_t kReadEvent;
	static const uint32_t kWriteEvent;

	void update();

	EventCallback _closeCallback;
	EventCallback _errorCallback;
	EventCallback _readCallback;
	EventCallback _writeCallback;
};

}

#endif	// TINYSERVER_CHANNEL_H
