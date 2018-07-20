#include "log.h"
#include "../../../include/unp.h"
#include "connection.h"
#include "control.h"

Connection::Connection() :
		_connfd(0)
{
}

Connection::Connection(int connfd) :
		_connfd(connfd)
{
	LOG_DEBUG("create connfd: " << connfd);
}

Connection::~Connection()
{
	LOG_DEBUG("destoried connfd: " << this->_connfd);
}

void Connection::setControl(Control *ctrl)
{
	this->_control = ctrl;
}

void Connection::run()
{
	ssize_t nread = 0;
	char line[MAXLINE];
	
	while(true) {
		LOG_DEBUG("read data begin, confd: " << this->getConnfd());
		if((nread = Read(this->_connfd, line, MAXLINE)) > 0) {
			LOG_DEBUG("write data begin, confd: " << this->getConnfd());
			Writen(this->_connfd, line, nread);
			LOG_DEBUG("write data over, confd: " << this->getConnfd());
		}

		if(nread < 0 && errno != EINTR) {
			LOG_ERROR("Read error: " << errno << "，confd: " << this->getConnfd());
		}

		if(nread == 0) {
			LOG_DEBUG("connection closed: " << this->getConnfd());
			return;
		}
	}
}
