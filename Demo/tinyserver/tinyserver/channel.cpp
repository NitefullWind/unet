#include <tinyserver/channel.h>

using namespace tinyserver;

Channel::Channel(int fd):
	_fd(fd)
{
}

Channel::~Channel()
{
}
