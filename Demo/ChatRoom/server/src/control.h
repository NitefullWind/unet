#ifndef CONTROL_H
#define CONTROL_H

#include "../../../include/unpthread.h"

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
	pthread_mutex_t *getLock();
private:
	ServerInfo _servInfo;
	int _nthreads;
	Thread *_tptr;
	pthread_mutex_t _mlock;
};

void *thread_main(void *arg);

#endif	// CONTROL_H
