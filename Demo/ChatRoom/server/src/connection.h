#ifndef CONNECTION_H
#define CONNECTION_H

class Control;

#define MAXLINE 4096

class Connection
{
public:
	Connection();
	Connection(int connfd);
	~Connection();

	void setControl(Control *);
	void run();

	int getConnfd() { return _connfd; }
private:
	int _connfd;
	Control *_control;
};

#endif //CONNECTION_H
