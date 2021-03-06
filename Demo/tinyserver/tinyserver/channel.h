#ifndef TINYSERVER_CHANNEL_H
#define TINYSERVER_CHANNEL_H

#include <functional>
#include <list>
#include <vector>

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

	short events() const { return  _events; }
	short revents() const { return _revents; }
	void setRevents(short revents) { _revents = revents; }

	size_t index() const { return _index; }
	void setIndex(size_t index) { _index = index; }

	void handleEvent();

	void enableReading() { if(!isReading()) {_events |= kReadEvent; update();} }
	void enableWriting() { if(!isWriting()) {_events |= kWriteEvent; update();} }
	void disableWriting() { if(isWriting()) {_events &= (short)~kWriteEvent; update();} }
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
	short _events;		// struct pollfd.events
	short _revents;		// struct pollfd.revents
	size_t _index;		// for pollfds in poller

	static const short kNoneEvent;
	static const short kReadEvent;
	static const short kWriteEvent;

	void update();

	EventCallback _closeCallback;
	EventCallback _errorCallback;
	EventCallback _readCallback;
	EventCallback _writeCallback;
};

typedef std::list<Channel *> ChannelList;
typedef std::vector<Channel *> ChannelVector;

}

#endif	// TINYSERVER_CHANNEL_H
