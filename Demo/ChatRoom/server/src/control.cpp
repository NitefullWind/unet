#include "control.h"
#include "connection.h"
#include <memory>

Control::Control(int argc, char **argv) :
		_servInfo(ServerInfo()),
		_nthreads(100),
		_listenfdMutex(PTHREAD_MUTEX_INITIALIZER),
		_threadsMutex(PTHREAD_MUTEX_INITIALIZER),
		_connectionsMutex(PTHREAD_MUTEX_INITIALIZER)
{
	if(argc == 2) {
		memcpy(_servInfo.port, argv[1], strlen(argv[1]));
	} else if(argc > 2) {
		memcpy(_servInfo.host, argv[1], strlen(argv[1]));
		memcpy(_servInfo.port, argv[2], strlen(argv[2]));
	}

	if(argc == 4) {
		_nthreads = atoi(argv[3]);
	}
}

Control::~Control()
{
	this->_threads.clear();
	this->_conntions.clear();
}

int Control::exec()
{
	printf("===== host:%s, port:%s=====\n", _servInfo.host, _servInfo.port);
	_servInfo.listenfd = Tcp_listen(_servInfo.host, _servInfo.port, &_servInfo.addrlen);

	autoAdjustThreadPool();
	
	while(true) {
		pause();
	}

	return 0;
}

void Control::autoAdjustThreadPool()
{
	Pthread_mutex_lock(&this->_connectionsMutex);
	size_t connSize = this->_conntions.size();
	Pthread_mutex_unlock(&this->_connectionsMutex);
	
	Pthread_mutex_lock(&this->_threadsMutex);
	if(this->_threads.size() - connSize <= this->_nthreads) {
		for(size_t i=0; i<this->_nthreads; i++) {
			auto pthr = std::make_shared<Thread>(this);
			this->_threads.push_back(pthr);
			pthread_t tid;
			Pthread_create(&tid, NULL, &thread_main, (void *)(pthr.get()));
		}
		printf("===== threads size: %lu\n", this->_threads.size());
	}
	Pthread_mutex_unlock(&this->_threadsMutex);
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
	autoAdjustThreadPool();
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

	Pthread_mutex_lock(&this->_connectionsMutex);
	size_t connSize = this->_conntions.size();
	Pthread_mutex_unlock(&this->_connectionsMutex);
	
	Pthread_mutex_lock(&this->_threadsMutex);
	if(this->_threads.size() - connSize > 2*this->_nthreads) {
		size_t count = 0;
		for(auto rit=this->_threads.crbegin(); rit!=this->_threads.crend() 
						&& count<this->_nthreads; rit++) {
			if(!(*rit)->isWorking()) {
				(*rit)->setIsLiving(false);
				//Pthread_join((*rit)->getThreadId(), NULL);
				auto it = (++rit).base(); //转成正序迭代器
				this->_threads.erase(it);
				count ++;
				continue;
			}
		}
		printf("===== threads size: %lu\n", this->_threads.size());
	}
	Pthread_mutex_unlock(&this->_threadsMutex);
}

void *thread_main(void *arg)
{
	auto pthr = (Thread *)arg;
	pthr->setThreadId(pthread_self());
	auto ctrl = pthr->getControl();

	int connfd = 0;
	socklen_t clilen = 0;
	struct sockaddr *cliaddr = NULL;

	while(pthr->isLiving()) {
		Pthread_mutex_lock(ctrl->getListenfdMutex());
		connfd = Accept(ctrl->getServerInfo()->listenfd, cliaddr, &clilen);
		pthr->setIsWorking(true);
		Pthread_mutex_unlock(ctrl->getListenfdMutex());
		
		std::shared_ptr<Connection> connPtr = ctrl->addConnection(connfd);
		connPtr->run();
		pthr->setIsWorking(false);
		ctrl->removeConnection(connfd);
	}
	
	return NULL;
}
