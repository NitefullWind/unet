#ifndef __UNP_H
#define __UNP_H

#ifndef HAVE_POLL
#define HAVE_POLL_H
#define HAVE_POLL				// 定义HAVE_POLL启用unp.h中的Poll函数
#endif

#ifndef HAVE_MKSTEMP
#define HAVE_MKSTEMP
#endif

#ifndef HAVE_SYS_IOCTL_H
#define HAVE_SYS_IOCTL_H
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
#include <sys/stropts.h>

#ifdef	HAVE_SYS_SELECT_H
# include	<sys/select.h>	/* for convenience */
#endif

#ifdef	HAVE_SYS_SYSCTL_H
#ifdef	HAVE_SYS_PARAM_H
# include	<sys/param.h>	/* OpenBSD prereq for sysctl.h */
#endif
# include	<sys/sysctl.h>
#endif

#ifdef	HAVE_POLL_H
# include	<poll.h>		/* for convenience */
#endif

#ifdef	HAVE_SYS_EVENT_H
# include	<sys/event.h>	/* for kqueue */
#endif

#ifdef	HAVE_STRINGS_H
# include	<strings.h>		/* for convenience */
#endif

/* Three headers are normally needed for socket/file ioctl's:
 *  * <sys/ioctl.h>, <sys/filio.h>, and <sys/sockio.h>.
 *   */
#ifdef	HAVE_SYS_IOCTL_H
# include	<sys/ioctl.h>
#endif
#ifdef	HAVE_SYS_FILIO_H
# include	<sys/filio.h>
#endif
#ifdef	HAVE_SYS_SOCKIO_H
# include	<sys/sockio.h>
#endif

#ifdef	HAVE_PTHREAD_H
# include	<pthread.h>
#endif

#ifdef HAVE_NET_IF_DL_H
# include	<net/if_dl.h>
#endif

#ifdef HAVE_NETINET_SCTP_H
#include	<netinet/sctp.h>
#endif


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


#define FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
					// default file access permissions for new files
#define DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
					// default permissions for new directories

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

/* The structure returned by recvfrom_flags */
struct unp_in_pkinfo {
	struct in_addr	ipi_addr;		// dst IPv4 address
	int				ipi_ifindex;	// received interface index
};

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
void		dg_cli(FILE*, int , const SA*, socklen_t);
void		dg_echo(int, SA*, socklen_t);
char		*gf_time(void);
struct		addrinfo *host_serv(const char *, const char *, int, int);
char		*sock_ntop(const SA *, socklen_t);
char		*sock_ntop_host(const SA *, socklen_t);
void		str_echo(int);
void		str_cli(FILE *, int);
int			tcp_connect(const char *, const char *);
int			tcp_listen(const char *, const char *, socklen_t *);


/* prototypes for out own library wrapper functions */
struct addrinfo *Host_serv(const char *, const char *, int, int);
const char *Inet_ntop(int, const void *, char *, size_t);
void		Inet_pton(int, const char *, void *);

typedef	void	Sigfunc(int);	/* for signal handlers */
Sigfunc *Signal(int, Sigfunc *);
Sigfunc *Signal_intr(int, Sigfunc *);
char	*Sock_ntop(const SA *, socklen_t);
char	*Sock_ntop_host(const SA *, socklen_t);
int		Tcp_connect(const char *, const char *);
int		Tcp_listen(const char *, const char *, socklen_t *);

/* prototypes for out Unix wrapper functions see {Sec errors} */
void *Calloc(size_t, size_t);
void Close(int);
void Dup2(int, int);
int Fcntl(int, int, int);
void Gettimeofday(struct timeval *, void *);
int Ioctl(int, int, void *);
pid_t Fork(void);
void *Malloc(size_t);
int Mkstemp(char *);
void *Mmap(void *, size_t, int, int, int, off_t);
int Open(const char *, int, mode_t);
void Pipe(int *fds);
ssize_t Read(int, void *, size_t);
void Sigaddset(sigset_t *, int);
void Sigdelset(sigset_t *, int);
void Sigemptyset(sigset_t *);
void Sigfillset(sigset_t *);
int Sigismember(const sigset_t *, int);
void Sigpending(sigset_t *);
void Sigprocmask(int, const sigset_t *, sigset_t *);
char *Strdup(const char *);
long Sysconf(int);
void Sysctl(int *, u_int, void *, size_t *, void *, size_t);
void Unlink(const char *);
pid_t Wait(int *);
pid_t Waitpid(pid_t, int *, int);
void Write(int, void *, size_t);

/* prototypes for out stdio wrapper functions: see {Sec errors} */
void	Fclose(FILE *);
FILE	*Fdopen(int, const char *);
char	*Fgets(char *, int, FILE *);
FILE	*Fopen(const char *, const char *);
void	Fputs(const char *, FILE *);


#define UNUSED(var)	(void)((var)=(var))

#endif
