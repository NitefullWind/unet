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
}

Connection::~Connection()
{
	printf("====destoried connfd: %d\n", this->_connfd);
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
		if((nread = Read(this->_connfd, line, MAXLINE)) > 0) {
			Writen(this->_connfd, line, nread);
		}

		if(nread < 0 && errno != EINTR) {
			err_sys("=====Read error: %d\n", errno);
		}

		if(nread == 0) {
			return;
		}
	}
}
