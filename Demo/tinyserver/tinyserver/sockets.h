#ifndef TINYSERVER_SOCKETS_H
#define TINYSERVER_SOCKETS_H

#include <tinyserver/logger.h>

#include <arpa/inet.h>	// htonl(), htons(), ntohl(), ntohs()
#include <endian.h>		// htobe64(), be64toh()
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace tinyserver
{
namespace sockets
{

inline uint64_t HostToNetwork64(uint64_t host64)
{
	return htobe64(host64);
}

inline uint32_t HostToNetwork32(uint32_t host32)
{
	return htonl(host32);
}

inline uint16_t HostToNetwork16(uint16_t host16)
{
	return htons(host16);
}

inline uint64_t NetworkToHost64(uint64_t net64)
{
	return be64toh(net64);
}

inline uint32_t NetworkToHost32(uint32_t net32)
{
	return ntohl(net32);
}

inline uint16_t NetworkToHost16(uint16_t net16)
{
	return ntohs(net16);
}

int Accept(int sockfd, struct sockaddr_in *addr);

void Bind(int sockfd, const struct sockaddr_in& addr);

void Close(int sockfd);

int CreateNonblockingSocket();

const char *Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
void Inet_pton(int family, const char *strptr, void *addrptr);

void Listen(int sockfd);

int Poll(struct pollfd *fds, unsigned long nfds, int timeout);

ssize_t Read(int fd, void *buf, size_t nbytes);
void Write(int fd, const void *buf, size_t nbytes);

void ShutdownWrite(int sockfd);

int Socket(int family, int type, int protocol);

void FromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr);
void ToHostPort(char *buf, size_t size, const struct sockaddr_in& addr);

}
}

#endif	// TINYSERVER_SOCKETS_H
