#ifndef TINYSERVER_SOCKETS_H
#define TINYSERVER_SOCKETS_H

#include <tinyserver/logger.h>

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace tinyserver
{
namespace sockets
{

int Poll(struct pollfd *fds, unsigned long nfds, int timeout);

int Socket(int family, int type, int protocol);

}
}

#endif	// TINYSERVER_SOCKETS_H
