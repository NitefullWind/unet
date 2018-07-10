#ifndef CONTROL_H
#define CONTROL_H

#include <map>
#include <list>
#include <memory>
#include "../../../include/unpthread.h"

class Connection;
class Thread;
class Control;

class Thread
{
public:
	Thread(Control *ctrl):
			_threadId(0),
			_control(ctrl),
			_isWorking(false),
			_isLiving(true)
	{
	}
	~Thread(){}

	void setThreadId(pthread_t tid) { this->_threadId = tid; }
	pthread_t getThreadId() { return _threadId; }
	Control * getControl() const { return _control; }
	void setIsWorking(bool isWorking) { this->_isWorking = isWorking; }
	bool isWorking() { return this->_isWorking; }
	void setIsLiving(bool isLiving) { this->_isLiving = isLiving; }
	bool isLiving() { return this->_isLiving; }
private:
	pthread_t _threadId;
	Control *_control;
	bool _isWorking;
	bool _isLiving;
};

typedef struct {
	char host[256] = "127.0.0.1";
	char port[8] = SERV_PORT_STR;
	int listenfd = 0;
	socklen_t addrlen = 0;
} ServerInfo;

class Control
{
public:
	Control(int argc, char **argv);
	~Control();

	int exec();


	const ServerInfo *getServerInfo() const;
	pthread_mutex_t *getListenfdMutex();

	std::shared_ptr<Connection> addConnection(int connfd);
	void addConnection(std::shared_ptr<Connection> connPtr);
	// 返回当前connections的拷贝
	std::map<int, std::shared_ptr<Connection> > getConnections();
	void removeConnection(int connfd);
private:
	void autoAdjustThreadPool();

private:
	ServerInfo _servInfo;
	size_t _nthreads;
	std::list<std::shared_ptr<Thread> > _threads;
	pthread_mutex_t _listenfdMutex, _threadsMutex, _connectionsMutex;

	std::map<int, std::shared_ptr<Connection> > _conntions;
};

void *thread_main(void *arg);

#endif	// CONTROL_H
