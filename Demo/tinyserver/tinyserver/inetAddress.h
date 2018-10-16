#ifndef TINYSERVER_INETADDRESS_H
#define TINYSERVER_INETADDRESS_H

#include <netinet/in.h>

namespace tinyserver
{

class InetAddress
{
public:
	explicit InetAddress(uint16_t port);
	InetAddress(const char *ip, uint16_t port);
	InetAddress(const struct sockaddr_in& addr);

private:
	struct sockaddr_in _addr;
};

}

#endif	// TINYSERVER_INETADDRESS_H
