#include "control.h"
#include "connection.h"
#include <memory>

Control::Control(int argc, char **argv) :
		_servInfo(ServerInfo()),
		_nthreads(100),
		_tptr(nullptr),
		_listenfdMutex(PTHREAD_MUTEX_INITIALIZER),
		_connectionsMutex(PTHREAD_MUTEX_INITIALIZER)
{
	if(argc == 2) {
		memcpy(_servInfo.port, argv[1], strlen(argv[1]));
	} else if(argc == 3) {
		memcpy(_servInfo.host, argv[1], strlen(argv[1]));
		memcpy(_servInfo.port, argv[2], strlen(argv[2]));
	}
}

Control::~Control()
{
	if(!_tptr) {
		free(_tptr);
	}
	this->_conntions.clear();
}

int Control::exec()
{
	printf("===== host:%s, port:%s=====\n", _servInfo.host, _servInfo.port);
	_servInfo.listenfd = Tcp_listen(_servInfo.host, _servInfo.port, &_servInfo.addrlen);
	
	_tptr = (Thread*)Calloc(_nthreads, sizeof(Thread));

	for(int i=0; i<_nthreads; i++) {
		Pthread_create(&this->_tptr[i].thread_tid, NULL, &thread_main, (void *)this);
	}

	while(true) {
		pause();
	}

	return 0;
}

const ServerInfo *Control::getServerInfo() const
{
	return &(this->_servInfo);
}

pthread_mutex_t *Control::getListenfdMutex()
{
	return &(this->_listenfdMutex);
}

std::shared_ptr<Connection> Control::addConnection(int connfd)
{
	std::shared_ptr<Connection> connPtr = std::make_shared<Connection>(connfd);
	connPtr->setControl(this);
	addConnection(connPtr);
	return connPtr;
}

void Control::addConnection(std::shared_ptr<Connection> connPtr)
{	
	Pthread_mutex_lock(&this->_connectionsMutex);
	this->_conntions[connPtr->getConnfd()] = connPtr;
	Pthread_mutex_unlock(&this->_connectionsMutex);

	printf("=====add connection: %d\n", connPtr->getConnfd());
}

std::map<int, std::shared_ptr<Connection> > Control::getConnections()
{	
	return _conntions;
}

void Control::removeConnection(int connfd)
{
	Pthread_mutex_lock(&this->_connectionsMutex);
	this->_conntions.erase(connfd);
	Pthread_mutex_unlock(&this->_connectionsMutex);

	Close(connfd);
	printf("=====remove connection: %d\n", connfd);
}

void *thread_main(void *arg)
{
	Control *ctrl = (Control *)arg;

	int connfd = 0;
	socklen_t clilen = 0;
	struct sockaddr *cliaddr = NULL;

	while(true) {
		Pthread_mutex_lock(ctrl->getListenfdMutex());
		connfd = Accept(ctrl->getServerInfo()->listenfd, cliaddr, &clilen);
		Pthread_mutex_unlock(ctrl->getListenfdMutex());
		
		std::shared_ptr<Connection> connPtr = ctrl->addConnection(connfd);
		connPtr->run();
	}
	
	return NULL;
}
