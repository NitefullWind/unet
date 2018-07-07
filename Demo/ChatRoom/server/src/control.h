#ifndef CONTROL_H
#define CONTROL_H

#include <map>
#include <memory>
#include "../../../include/unpthread.h"

class Connection;

typedef struct {
	pthread_t thread_tid;
} Thread;

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
	ServerInfo _servInfo;
	int _nthreads;
	Thread *_tptr;
	pthread_mutex_t _listenfdMutex, _connectionsMutex;

	std::map<int, std::shared_ptr<Connection> > _conntions;
};

void *thread_main(void *arg);

#endif	// CONTROL_H
