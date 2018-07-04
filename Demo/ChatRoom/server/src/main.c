#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, addrlen=0;
	if(argc == 1) {
		listenfd = Tcp_listen(NULL, SERV_PORT_STR, &addrlen);
	} else if(argc == 2) {
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	} else if(argc == 3) {
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	}
	
	while(1) {
	}
	
	return 0;
}
