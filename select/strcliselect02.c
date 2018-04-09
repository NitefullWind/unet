#include "../lib/unp.h"

void str_cli(FILE *fd, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

	stdineof = 0;
	FD_ZERO(&rset);
	for(;;)
	{
		if(stdineof == 0) {
			FD_SET(fileno(fd), &rset);
		}
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fd), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)) {
			if((n == Read(sockfd, buf, MAXLINE)) == 0) {
				if(stdineof == 1)
					return;							// normal termination
				else
					err_quit("str_cli: server terminated prematurely");
			}
			Write(fileno(stdout), buf, n);
		}

		if(FD_ISSET(fileno(fd), &rset)) {
			if((n = Read(fileno(fd), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);			// send FIN
				FD_CLR(fileno(fd), &rset);
				continue;
			}
			Writen(sockfd, buf, n);
		}
	}
}
