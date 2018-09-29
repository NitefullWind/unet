#ifndef MUDUO_NET_INETADDRESS_H
#define MUDUO_NET_INETADDRESS_H

#include <netinet/in.h>
#include <string>

namespace muduo
{
namespace net
{

class InetAddress
{
public:
	explicit InetAddress(uint16_t port);
	
	InetAddress(const std::string& ip, uint16_t port);
	
	InetAddress(const struct sockaddr_in& addr):
		_addr(addr)
	{
	}

	std::string toHostPort() const;

	const struct sockaddr_in& socketAddrInet() const { return _addr; }
	void setSocketAddrInet(const struct sockaddr_in& addr) { _addr = addr; }

private:
	struct sockaddr_in _addr;
};

}
}


#endif // MUDUO_NET_INETADDRESS_H
