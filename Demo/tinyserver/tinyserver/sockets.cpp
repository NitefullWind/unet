#include <tinyserver/sockets.h>

using namespace tinyserver;

const char *sockets::Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char *ptr;
	if(strptr == nullptr) {
		LOG_FATAL("NULL 3rd argument to inet_ntop");
	}
	if((ptr = ::inet_ntop(family, addrptr, strptr, static_cast<socklen_t>(len))) == nullptr) {
		LOG_FATAL("inet_ntop error");
	}
	return ptr;
}

void sockets::Inet_pton(int family, const char *strptr, void *addrptr)
{
	int n;
	if((n = ::inet_pton(family, strptr, addrptr)) <= 0) {
		LOG_FATAL("inet_pton error for " << strptr);
	}
}

int sockets::Poll(struct pollfd *fds, unsigned long nfds, int timeout)
{
	int n;
	if((n = ::poll(fds, nfds, timeout)) < 0) {
		LOG_FATAL("poll error");
	}
	return n;
}

int sockets::Socket(int family, int type, int protocol)
{
	int n;
	if((n = ::socket(family, type, protocol)) < 0) {
		LOG_FATAL("socket error")
	}
	return n;
}
