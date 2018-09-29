#include <muduo/net/inetAddress.h>
#include <muduo/net/socketsOps.h>

#include <strings.h>				// bzero()

#include <assert.h>

using namespace muduo;
using namespace muduo::net;

static const in_addr_t kInaddrAny = INADDR_ANY;

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in), 
		"size of InetAddress and struct sockaddr_in is not equal");

InetAddress::InetAddress(uint16_t port)
{
	bzero(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
	_addr.sin_port = sockets::hostToNetwork16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	bzero(&_addr, sizeof(_addr));
	sockets::fromHostPort(ip.c_str(), port, &_addr);
}

std::string InetAddress::toHostPort() const 
{
	char buf[32];
	sockets::toHostPort(buf, sizeof(buf),  _addr);
	return buf;
}
