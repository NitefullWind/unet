#include <tinyserver/sockets.h>

using namespace tinyserver;

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
