#include <tinyserver/eventLoop.h>
#include <tinyserver/channel.h>
#include <tinyserver/inetAddress.h>
#include <tinyserver/logger.h>

using namespace tinyserver;

EventLoop::EventLoop() :
	_looping(false),
	_stop(false)
{
}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
	_looping = true;

	for(int i=0; i<static_cast<int>(_channelVector.size()); ++i) {
		int fd = _channelVector[i]->fd();
		if(_fdset.find(fd) == _fdset.end()) {
			struct pollfd pollfd;
			pollfd.fd = fd;
			pollfd.events = POLLRDNORM;
			_fds.push_back(pollfd);
			_fdset.insert(fd);
		}
	}

	while(!_stop) {
		int n = sockets::Poll(_fds.data(), _fds.size(), 10 * 1000);
		if(n > 0) {
			for(auto fd : _fds) {
				if(fd.revents & POLLRDNORM) {	// new client connection
					struct sockaddr_in sockaddr;
					int clientfd = sockets::Accept(fd.fd, &sockaddr);
					if(clientfd > 0) {
						InetAddress clientAddr(sockaddr);
						LOG_DEBUG("new connection: " << clientfd << " from: " << clientAddr.toHostPort());
					}
				} else if (fd.revents & POLLERR) {
					LOG_ERROR("POLLERR");
				} else if (fd.revents & POLLHUP) {
					LOG_ERROR("POLLHUP");
				} else if (fd.revents & POLLNVAL) {
					LOG_ERROR("POLLNVAL");
				}
			}
		} else if (n == 0) {
			LOG_TRACE("poll nothing happend");
		} else {
			LOG_ERROR("Poll return error: " << n);
		}
	}

	_looping = false;
}

void EventLoop::stop()
{
	_stop = true;
}

void EventLoop::addChannel(Channel *channel)
{
	_channelVector.push_back(channel);
}
