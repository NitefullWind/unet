#ifndef MUDUO_NET_SOCKET_H
#define MUDUO_NET_SOCKET_H

namespace muduo
{
namespace net
{

class InetAddress;

class Socket
{
public:
	explicit Socket(int sockfd):
		_sockfd(sockfd)
	{
	}
	
	~Socket();

	int fd() const { return _sockfd; }

	void bindAddress(const InetAddress& localaddr);
	void listen();

	int accept(InetAddress *peeraddr);

	void setReuseAddr(bool on);
private:
	const int _sockfd;
};

}
}

#endif // MUDUO_NET_SOCKET_H
