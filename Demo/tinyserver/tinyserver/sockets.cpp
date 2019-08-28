#include <tinyserver/sockets.h>

#include <fcntl.h>
#include <strings.h>
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
		TLOG_FATAL("accept error");
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
				TLOG_FATAL("unexpected error of ::accept " << savedError);
				break;
			default:
				TLOG_FATAL("unknown error of ::accept " << savedError);
				break;
		}
	}
	return connfd;
}

void sockets::Bind(int sockfd, const struct sockaddr_in& addr)
{
	if(::bind(sockfd, SockaddrCast(&addr), sizeof(addr)) < 0) {
		TLOG_FATAL("bind error");
		abort();
	}
}

void sockets::Close(int sockfd)
{
	if(::close(sockfd) < 0) {
		TLOG_FATAL("close error");
	}
}

int sockets::Connect(int sockfd, const struct sockaddr_in& addr)
{
	return ::connect(sockfd, SockaddrCast(&addr), sizeof(addr));
}

int sockets::CreateNonblockingSocket()
{
	// socket
#if VALGRIND
	int sockfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0) {
		TLOG_FATAL(__FUNCTION__ << " failed");
	}
	SetNonBlockAndCloseOnExec(sockfd);
#else
	int sockfd = Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if(sockfd < 0) {
		TLOG_FATAL(__FUNCTION__ << " failed");
	}
#endif
	return sockfd;
}

const char *sockets::Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char *ptr;
	if(strptr == nullptr) {
		TLOG_FATAL("NULL 3rd argument to inet_ntop");
	}
	if((ptr = ::inet_ntop(family, addrptr, strptr, static_cast<socklen_t>(len))) == nullptr) {
		TLOG_FATAL("inet_ntop error");
	}
	return ptr;
}

void sockets::Inet_pton(int family, const char *strptr, void *addrptr)
{
	if(::inet_pton(family, strptr, addrptr) <= 0) {
		TLOG_FATAL("inet_pton error for " << strptr);
	}
}

void sockets::Listen(int sockfd)
{
	if(::listen(sockfd, SOMAXCONN) < 0) {
		TLOG_FATAL("listen error");
	}
}

int sockets::Poll(struct pollfd *fds, unsigned long nfds, int timeout)
{
	int n;
	if((n = ::poll(fds, nfds, timeout)) < 0) {
		TLOG_FATAL("poll error");
	}
	return n;
}

ssize_t sockets::Read(int fd, void *buf, size_t nbytes)
{
	ssize_t n;
	if((n = ::read(fd, buf, nbytes)) == -1) {
		TLOG_FATAL("read error");
	}
	return n;
}

ssize_t sockets::Write(int fd, const void *buf, size_t nbytes)
{
	ssize_t n;
	if((n = ::write(fd, buf, nbytes)) != (ssize_t)nbytes) {
		TLOG_FATAL("write error");
	}
	return n;
}

ssize_t sockets::Writen(int fd, const void *buf, size_t nbytes)
{
	ssize_t nwritten = 0;
	while((size_t)nwritten < nbytes) {
		ssize_t n = ::write(fd, buf, nbytes);
		if(n > 0) {
			nwritten += n;
		} else if (n < 0 && errno != EINTR) {		// ignore error(s): EINTR
			TLOG_FATAL("write error");
			return -1;
		}
	}
	return nwritten;
}

void sockets::ShutdownWrite(int sockfd)
{
	if(::shutdown(sockfd, SHUT_WR) < 0) {
		TLOG_FATAL("shudown SHUT_WR error");
	}
}

int sockets::Socket(int family, int type, int protocol)
{
	int n;
	if((n = ::socket(family, type, protocol)) < 0) {
		TLOG_FATAL("socket error")
	}
	return n;
}

void sockets::FromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = sockets::HostToNetwork16(port);
	if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
		TLOG_FATAL("inet_pton in FromHostPort error");
	}
}

void sockets::ToHostPort(char *buf, size_t size, const struct sockaddr_in& addr)
{
	char host[INET_ADDRSTRLEN] = "INVALID";
	::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	uint16_t port = sockets::NetworkToHost16(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}

sockaddr_in sockets::GetLocalAddr(int sockfd)
{
	struct sockaddr_in localAddr;
	socklen_t addrlen = sizeof(localAddr);
	bzero(&localAddr, addrlen);
	if(::getsockname(sockfd, SockaddrCast(&localAddr), &addrlen) < 0) {
		TLOG_FATAL("getsockname error");
	}
	return localAddr;
}

sockaddr_in sockets::GetPeerAddr(int sockfd)
{
	struct sockaddr_in peerAddr;
	socklen_t addrlen = sizeof(peerAddr);
	bzero(&peerAddr, addrlen);
	if(::getpeername(sockfd, SockaddrCast(&peerAddr), &addrlen) < 0) {
		TLOG_FATAL("getpeername error");
	}
	return peerAddr;
}

int sockets::getSocketError(int sockfd)
{
	int optval;
	socklen_t optlen = sizeof(optval);

	if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		return errno;
	} else {
		return optval;
	}
}

#if !(__GNUC_PREREQ (4,6))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
bool sockets::IsSelfConnect(int sockfd)
{
	struct sockaddr_in localaddr = GetLocalAddr(sockfd);
	struct sockaddr_in peeraddr = GetPeerAddr(sockfd);
	if (localaddr.sin_family == AF_INET)
	{
		return localaddr.sin_port == peeraddr.sin_port
			&& localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
	}
	else
	{
		return false;
	}
}