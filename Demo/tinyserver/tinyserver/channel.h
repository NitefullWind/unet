#ifndef TINYSERVER_CHANNEL_H
#define TINYSERVER_CHANNEL_H

namespace tinyserver
{

class Channel
{
public:
	explicit Channel(int fd);
	~Channel();

	int fd() const { return _fd; }

private:
	const int _fd;
};

}

#endif	// TINYSERVER_CHANNEL_H
