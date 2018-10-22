#ifndef TINYSERVER_TCPCONNECTION_H
#define TINYSERVER_TCPCONNECTION_H

#include <functional>
#include <memory>

namespace tinyserver
{

class Channel;
class EventLoop;

class TcpConnection
{
public:
	typedef std::function<void(size_t)> CloseCallback;

	explicit TcpConnection(EventLoop *loop, int sockfd);
	~TcpConnection();

	size_t index() const { return _index; }
	void setIndex(size_t index) { _index = index; }

	void setCloseCallback(const CloseCallback& cb) {
		_closeCallback = cb;
	}

	void onClose();
	void onReading();
	void onWriting();
private:
	EventLoop *_loop;
	std::unique_ptr<Channel> _channel;
	size_t _index;

	CloseCallback _closeCallback;
};

}

#endif // TINYSERVER_TCPCONNECTION_H
