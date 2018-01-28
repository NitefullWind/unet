#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	void sig_chld(int);
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	Signal(SIGCHLD, sig_chld);	// must call waitpid()

	for(;;)
	{
		clilen = sizeof(cliaddr);
		if ( (connfd = Accept(listenfd, (SA*) &cliaddr, &clilen)) < 0) {
			if(errno == EINTR) {
				continue;		// back to for()
			} else {
				err_sys("accept error");
			}
		}

		if((childpid = Fork()) == 0) {
			char peerIp[] = "000.000.000.000";
			Inet_ntop(AF_INET, &cliaddr.sin_addr, peerIp, sizeof(peerIp));
			printf("New client(%s:%d) connected.\n", peerIp, ntohs(cliaddr.sin_port));

			Close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
	return 0;
}
