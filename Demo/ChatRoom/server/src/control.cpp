#include "control.h"

Control::Control(int argc, char **argv) :
		_servInfo(ServerInfo()),
		_nthreads(100),
		_tptr(nullptr),
		_mlock(PTHREAD_MUTEX_INITIALIZER)
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

pthread_mutex_t *Control::getLock()
{
	return &(this->_mlock);
}

void *thread_main(void *arg)
{
	Control *ctrl = (Control *)arg;

	int connfd = 0;
	socklen_t clilen = 0;
	struct sockaddr *cliaddr = NULL;

	while(true) {
		Pthread_mutex_lock(ctrl->getLock());
		connfd = Accept(ctrl->getServerInfo()->listenfd, cliaddr, &clilen);
		Pthread_mutex_unlock(ctrl->getLock());

		printf("==== connected: %d\n", connfd);
		sleep(1);
		Close(connfd);
	}
	
	return NULL;
}
