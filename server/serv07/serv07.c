/*
 * TCP并发服务器程序，预先创建线程的服务器程序
 */

#include "../../lib/unpthread.h"
#include "pthread07.h"

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
	int i;
	void sig_int(int), thread_make(int);

	if(argc == 3) {
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	} else if(argc == 4) {
		listenfd = Tcp_listen(argv[1], argv[2],  &addrlen);
	} else {
		err_quit("usage: serv07 [ <host> ] <port#> <#threads>");
	}
	nthreads = atoi(argv[argc-1]);
	tptr = Calloc(nthreads, sizeof(Thread));

	for(i=0; i<nthreads; i++) {
		thread_make(i);												// only main thread returns
	}

	Signal(SIGINT, sig_int);

	for(;;) {
		pause();													// everything done by threads
	}
}

void sig_int(int signo)
{
	UNUSED(signo);

	void pr_cpu_time(void);

	pr_cpu_time();
	exit(0);
}
