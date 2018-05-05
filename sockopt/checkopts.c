/*
 * 使用getsockopt检查机器是否支持套接字选项，并获取默认值
 */

#include "../lib/unp.h"
#include <netinet/tcp.h>		// for TCP_XXX defines

union val {
	int				i_val;
	long			l_val;
	struct linger	linger_val;
	struct timeval	timeval_val;
} val;

static char *sock_str_flag(union val *, int);
static char *sock_str_int(union val *, int);
static char *sock_str_linger(union val *, int);
static char *sock_str_timeval(union val *, int);

struct sock_opts {
	const char*	opt_str;
	int			opt_level;
	int			opt_name;
	char*		(*opt_val_str)(union val *, int);
} sock_opts[] = {	
	{"SO_BROADCAST",		SOL_SOCKET,	SO_BROADCAST,		sock_str_flag},
	{"SO_DEBUG",			SOL_SOCKET,	SO_DEBUG,			sock_str_flag},
	{"SO_DONTROUTE",		SOL_SOCKET,	SO_DONTROUTE,		sock_str_flag},
	{"SO_ERROR",			SOL_SOCKET,	SO_ERROR,			sock_str_int},
	{"SO_KEEPALIVE",		SOL_SOCKET,	SO_KEEPALIVE,		sock_str_flag},
	{"SO_LINGER",			SOL_SOCKET,	SO_LINGER,			sock_str_linger},
	{"SO_OOBINLINE",		SOL_SOCKET,	SO_OOBINLINE,		sock_str_flag},
	{"SO_RECVBUF",			SOL_SOCKET,	SO_RCVBUF,			sock_str_int},
	{"SO_SENDBUF",			SOL_SOCKET,	SO_SNDBUF,			sock_str_int},
	{"SO_RCVLOWAT",			SOL_SOCKET,	SO_RCVBUF,			sock_str_int},
	{"SO_SNDLOWAT",			SOL_SOCKET,	SO_SNDLOWAT,		sock_str_int},
	{"SO_RCVTIMEO",			SOL_SOCKET,	SO_RCVTIMEO,		sock_str_timeval},
	{"SO_SNDTIMEO",			SOL_SOCKET,	SO_SNDTIMEO,		sock_str_timeval},
	{"SO_REUSEADDR",		SOL_SOCKET,	SO_REUSEADDR,		sock_str_flag},
#ifdef SO_REUSEPORT
	{"SO_REUSEPORT",		SOL_SOCKET,	SO_REUSEPORT,		sock_str_flag},
#else
	{"SO_REUSEPORT",		0,			0,					NULL},
#endif
	{"SO_TYPE",				SOL_SOCKET,	SO_TYPE,			sock_str_int},
#ifdef SO_USELOOPBACK
	{"SO_USELOOPBACK",		SOL_SOCKET,	SO_USELOOPBACK,		sock_str_flag},
#else
	{"SO_USELOOPBACK",		0,			0,					NULL},
#endif
#ifdef SO_TOS
	{"SO_TOS",				IPPROTO_IP,	SO_TOS,				sock_str_int},
#else
	{"SO_TOS",				0,			0,					NULL},
#endif
#ifdef SO_TTL
	{"SO_TTL",				IPPROTO_IP,	SO_TTL,				sock_str_int},
#else
	{"SO_TTL",				0,			0,					NULL},
#endif
#ifdef SO_DONTFRAG
	{"SO_DONTFRAG",			IPPROTO_IPV6,	SO_DONTFRAG,	sock_str_flag},
#else
	{"SO_DONTFRAG",			0,			0,					NULL},
#endif
#ifdef SO_UNICAST_HOPS
	{"SO_UNICAST_HOPS",		IPPROTO_IPV6,	SO_UNICAST_HOPS,		sock_str_int},
#else
	{"SO_UNICAST_HOPS",		0,			0,					NULL},
#endif
#ifdef SO_V6ONLY
	{"SO_V6ONLY",			IPPROTO_IPV6,	SO_V6ONLY,		sock_str_flag},
#else
	{"SO_V6ONLY",			0,			0,					NULL},
#endif
#ifdef SO_MAXSEG
	{"SO_MAXSEG",			IPPROTO_TCP,	SO_MAXSEG,		sock_str_int},
#else
	{"SO_MAXSEG",			0,			0,					NULL},
#endif
#ifdef SO_NODELAY
	{"SO_NODELAY",			IPPROTO_TCP,	SO_NODELAY,		sock_str_flag},
#else
	{"SO_NODELAY",			0,			0,					NULL},
#endif
#ifdef SO_AUTOCLOSE
	{"SO_AUTOCLOSE",		IPPROTO_SCTP,	SO_AUTOCLOSE,		sock_str_int},
#else
	{"SO_AUTOCLOSE",		0,			0,					NULL},
#endif
#ifdef SO_MAXBURST
	{"SO_MAXBURST",			IPPROTO_SCTP,	SO_MAXBURST,		sock_str_int},
#else
	{"SO_MAXBURST",			0,			0,					NULL},
#endif
#ifdef SO_MAXSEG
	{"SO_MAXSEG",			IPPROTO_SCTP,	SO_MAXSEG,		sock_str_int},
#else
	{"SO_MAXSEG",			0,			0,					NULL},
#endif
#ifdef SO_NODELAY 
	{"SO_NODELAY",			IPPROTO_SCTP,	SO_NODELAY,		sock_str_flag},
#else
	{"SO_NODELAY",			0,			0,					NULL},
#endif
	{NULL,					0,			0,					NULL}
};

int main(int argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);

	int fd;
	socklen_t len;
	struct sock_opts* ptr;

	for(ptr = sock_opts; ptr->opt_str!=NULL; ptr++) {
		printf("%s: \t", ptr->opt_str);
		if(ptr->opt_val_str == NULL) {
			printf("(undefined)\n");
		} else {
			switch(ptr->opt_level) {
			case SOL_SOCKET:
			case IPPROTO_IP:
			case IPPROTO_TCP:
				fd = Socket(AF_INET, SOCK_STREAM, 0);
				break;
#ifdef IPV6
			case IPPROTO_IPV6:
				fd = Socket(AF_INET6, SOCK_STREAM, 0);
				break;
#endif
#ifdef IPPROTO_SCTP
			case IPPROTO_SCTP:
				fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
				break;
#endif
			default:
				err_quit("Can't create fd for level %d:\n", ptr->opt_level);
			}

			len = sizeof(val);
			if(getsockopt(fd, ptr->opt_level, ptr->opt_name, &val, &len) == -1) {
				err_ret("getsockopt error");
			} else {
				printf("default = %s\n", (*ptr->opt_val_str)(&val, len));
			}
			close(fd);
		}
	}
	exit(0);
}

static char strres[128];

static char* sock_str_flag(union val *ptr, int len)
{
	if(len != sizeof(int)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	} else {
		snprintf(strres, sizeof(strres), "%s", (ptr->i_val == 0) ? "off" : "on");
	}
	return(strres);
}

static char* sock_str_int(union val *ptr, int len)
{
	if(len != sizeof(int)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
	} else {
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	}
	return(strres);
}

static char* sock_str_linger(union val *ptr, int len)
{
	if(len != sizeof(struct linger)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)", len);
	} else {
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d", ptr->linger_val.l_onoff, ptr->linger_val.l_linger);
	}
	return(strres);
}

static char* sock_str_timeval(union val *ptr, int len)
{
	if(len != sizeof(struct timeval)) {
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct timeval)", len);
	} else {
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec", (long)ptr->timeval_val.tv_sec, (long)ptr->timeval_val.tv_usec);
	}
	return(strres);
}
