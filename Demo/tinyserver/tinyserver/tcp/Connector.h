#ifndef TINYSERVER_CONNECTOR_H
#define TINYSERVER_CONNECTOR_H

#include <functional>
#include <memory>
#include <tinyserver/inetAddress.h>

namespace tinyserver
{

class Channel;
class EventLoop;

class Connector
{
public:
	typedef std::function<void (int sockfd)> NewConnectionCallback;

	explicit Connector(EventLoop *loop, const InetAddress& inetAddress);
	~Connector();

	// start to connect
	void start();
	// restart to connect
	void restart();
	// stop to connect
	void stop();

	const InetAddress& serverAddress() { return _serverAddress; }

	void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }
private:
	enum StateEnum { kConnecting, kConnected, kDisconnecting, kDisconnected };

	EventLoop *_loop;
	bool _connect;
	StateEnum _state;
	std::unique_ptr<Channel> _channel;
	InetAddress _serverAddress;
	NewConnectionCallback _newConnectionCallback;

	void startInLoop();
	void restartInLoop();
	void stopInLoop();
	void connect();
	void connecting(int sockfd);
	void retry(int sockfd);
	void onWrite();
	void onError();
	// 从loop中移除channel，删除channel指针，并返回当前channel的sockfd
	int removeAndResetChannel();

	void setState(StateEnum state) { this->_state = state; }
};
} // namespace tinyserver


#endif // TINYSERVER_CONNECTOR_H