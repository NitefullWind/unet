#include <tinyserver/inetAddress.h>
#include <tinyserver/sockets.h>

#include <strings.h>

using namespace tinyserver;

InetAddress::InetAddress(uint16_t port)
{
	bzero(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = sockets::HostToNetwork32(INADDR_ANY);
	_addr.sin_port = sockets::HostToNetwork16(port);
}

InetAddress::InetAddress(const char *ip, uint16_t port)
{
	bzero(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = sockets::HostToNetwork16(port);
	sockets::Inet_pton(_addr.sin_family, ip, &_addr.sin_addr.s_addr);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	InetAddress(ip.data(), port);
}

InetAddress::InetAddress(const struct sockaddr_in& addr) :
	_addr(addr)
{
}

std::string InetAddress::toHostPort() const
{
	char buf[32];
	sockets::ToHostPort(buf, sizeof(buf), _addr);
	return buf;
}
