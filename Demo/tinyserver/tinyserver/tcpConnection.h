#ifndef TINYSERVER_TCPCONNECTION_H
#define TINYSERVER_TCPCONNECTION_H

#include <memory>

namespace tinyserver
{

class Channel;
class EventLoop;

class TcpConnection
{
public:
	explicit TcpConnection(EventLoop *loop, int sockfd);
	~TcpConnection();

	void onClose();
	void onReading();
	void onWriting();
private:
	std::unique_ptr<Channel> _channel;
};

}

#endif // TINYSERVER_TCPCONNECTION_H
