#ifndef TINYSERVER_INETADDRESS_H
#define TINYSERVER_INETADDRESS_H

#include <netinet/in.h>
#include <string>

namespace tinyserver
{

class InetAddress
{
public:
	explicit InetAddress(uint16_t port);
	InetAddress(const char *ip, uint16_t port);
	InetAddress(const std::string& ip, uint16_t port);
	InetAddress(const struct sockaddr_in& addr);

	std::string toHostPort() const;

	const struct sockaddr_in& sockAddrInet() const { return _addr; }
	void setSockAddrInet(const struct sockaddr_in& addr) { _addr = addr; }

private:
	struct sockaddr_in _addr;
};

}

#endif	// TINYSERVER_INETADDRESS_H
