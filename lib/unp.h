#ifndef __UNP_H
#define __UNP_H

#ifndef HAVE_POLL
#define HAVE_POLL				// 定义HAVE_POLL启用unp.h中的Poll函数
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <stdarg.h>
#include <stdlib.h>

#include <limits.h>				// for OPEN_MAX
#include <poll.h>
#include <sys/stropts.h>

#ifndef SYMBOL
#define SYMBOL value
#endif

#ifndef INFTIM
#define INFTIM -1				// <poll.h>和<sys/stropts.h>中没有定义，自己定义
#endif

#ifndef OPEN_MAX
#define OPEN_MAX 1024			// <limits.h>中没有定义，自己定义
#endif

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually suppoeting many more */
#define LISTENQ		1024	/* 2nd argument to listen() */

/* Miscellaneous constans */
#define MAXLINE		4096	/* max text line length */
#define MAXSOCKADDR	128		/* max socket address structure size */
#define BUFFSIZE	8192	/* buffer size for reads and writes */

/* Define some port number that can be used for client-servers */
#define SERV_PORT		9877
#define SERV_PORT_STR	"9877"
#define UNIXSTR_PATH	"/tmp/unix.str"
#define UNIXDG_PATH		"/tmp/unix.dg"

/* Following shortens all the typecasts of pointer arguments: */
#define SA	struct sockaddr

void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);


#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

/* prototypes for out socket wrapper functions: see {Sec errors} */
int		Accept(int, SA *, socklen_t *);
void	Bind(int, const SA *, socklen_t);
void	Connect(int, const SA *, socklen_t);
void	Getpeername(int, SA *, socklen_t *);
void	Getsockname(int, SA *, socklen_t *);
void	Getsockopt(int, int, int, void *, socklen_t *);
int		Isfdtype(int, int);
void	Listen(int, int);
#ifdef	HAVE_POLL
int		Poll(struct pollfd *, unsigned long, int);
#endif
ssize_t	Readline(int, void *, size_t);
ssize_t	Readn(int, void *, size_t);
ssize_t Recv(int, void *, size_t, int);
ssize_t Recvfrom(int, void *, size_t, int, SA *, socklen_t *);
ssize_t Recvmsg(int, struct msghdr *, int);
int		Select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
void	Send(int, const void *, size_t, int);
void	Sendto(int, const void *, size_t, int, const SA *, socklen_t);
void	Sendmsg(int, const struct msghdr *, int);
void	Setsockopt(int, int, int, const void *, socklen_t);
void	Shutdown(int, int);
int		Sockatmark(int);
int		Socket(int, int, int);
void	Socketpair(int, int, int, int *);
void	Writen(int, void *, size_t);

ssize_t Read(int, void *, size_t);
void Write(int, void *, size_t);
void Close(int);
/* prototypes for out own library functions */
void		str_echo(int);
void		str_cli(FILE *, int);

/* prototypes for out stdio wrapper functions: see {Sec errors} */
void	Fclose(FILE *);
FILE	*Fdopen(int, const char *);
char	*Fgets(char *, int, FILE *);
FILE	*Fopen(const char *, const char *);
void	Fputs(const char *, FILE *);

/* prototypes for out own library wrapper functions */
const char *Inet_ntop(int, const void *, char *, size_t);
void		Inet_pton(int, const char *, void *);

typedef	void	Sigfunc(int);	/* for signal handlers */
Sigfunc *Signal(int, Sigfunc *);
Sigfunc *Signal_intr(int, Sigfunc *);


pid_t Fork(void);


#define UNUSED(var)	(void)((var)=(var))

#endif
