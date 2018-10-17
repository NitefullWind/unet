#include <tinyserver/sockets.h>

#include <fcntl.h>
#include <unistd.h>

using namespace tinyserver;

namespace
{

typedef struct sockaddr SA;

const SA *SockaddrCast(const struct sockaddr_in *addr)
{
	return static_cast<const SA*>((const void*)addr);
}

SA *SockaddrCast(struct sockaddr_in *addr)
{
	return static_cast<SA*>((void*)addr);
}

#if VALGRIND || defined (NO_ACCEPT4)
void SetNonBlockAndCloseOnExec(int sockfd)
{
	// non-block
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);

	// close-on-exec
	flags = ::fcntl(sockfd, F_GETFL, 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFL, flags);

	(void)ret;
}
#endif

}	// end namespace

int sockets::Accept(int sockfd, struct sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(*addr);
#if VALGRIND
	int connfd = ::accept(sockfd, SockaddrCast(addr), &addrlen);
	SetNonBlockAndCloseOnExec(connfd);
#else
	int connfd = ::accept4(sockfd, SockaddrCast(addr), &addrlen,
			SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
	if(connfd < 0) {
		int savedError = errno;
		LOG_FATAL("accept error");
		switch(savedError)
		{
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EMFILE:
				// expected errors
				errno = savedError;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				// unexpected errors
				LOG_FATAL("unexpected error of ::accept " << savedError);
				break;
			default:
				LOG_FATAL("unknown error of ::accept " << savedError);
				break;
		}
	}
	return connfd;
}

void sockets::Bind(int sockfd, const struct sockaddr_in& addr)
{
	if(::bind(sockfd, SockaddrCast(&addr), sizeof(addr)) < 0) {
		LOG_FATAL("bind error");
	}
}

void sockets::Close(int sockfd)
{
	if(::close(sockfd) < 0) {
		LOG_FATAL("close error");
	}
}

int sockets::CreateNonblockingSocket()
{
	// socket
#if VALGRIND
	int sockfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0) {
		LOG_FATAL(__FUNCTION__ << " failed");
	}
	SetNonBlockAndCloseOnExec(sockfd);
#else
	int sockfd = Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if(sockfd < 0) {
		LOG_FATAL(__FUNCTION__ << " failed");
	}
#endif
	return sockfd;
}

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
	if(::inet_pton(family, strptr, addrptr) <= 0) {
		LOG_FATAL("inet_pton error for " << strptr);
	}
}

void sockets::Listen(int sockfd)
{
	if(::listen(sockfd, SOMAXCONN) < 0) {
		LOG_FATAL("listen error");
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

ssize_t sockets::Read(int fd, void *buf, size_t nbytes)
{
	ssize_t n;
	if((n = ::read(fd, buf, nbytes)) == -1) {
		LOG_FATAL("read error");
	}
	return n;
}

void sockets::ShutdownWrite(int sockfd)
{
	if(::shutdown(sockfd, SHUT_WR) < 0) {
		LOG_FATAL("shudown SHUT_WR error");
	}
}

int sockets::Socket(int family, int type, int protocol)
{
	int n;
	if((n = ::socket(family, type, protocol)) < 0) {
		LOG_FATAL("socket error")
	}
	return n;
}
