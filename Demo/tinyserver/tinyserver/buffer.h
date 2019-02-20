#ifndef TINYSERVER_BUFFER_H
#define TINYSERVER_BUFFER_H

#include <string>
#include <vector>

namespace tinyserver
{

class Buffer
{
public:
	static const size_t kInitialSize = 1024;
	Buffer();
	~Buffer();

	size_t size() const { return _buffer.size(); }
	size_t readableBytes() const { return _writerIndex - _readerIndex; }
	size_t writeableBytes() const { return _buffer.size() - _writerIndex; }

	void append(const char *buf, size_t len);
	void append(const std::string& str);
	ssize_t readFd(int fd);

	std::string readAll();

private:
	std::vector<char> _buffer;
	size_t _writerIndex;
	size_t _readerIndex;

	// first char of the buffer
	char *begin() { return _buffer.data(); }
	const char *begin() const { return _buffer.data(); }

	// first char of the buffer that can write
	char *beginWrite() { return begin() + _writerIndex; }
	const char *beginWrite() const { return begin() + _writerIndex; }

	// first char of the buffer that can read
	char *peek() { return begin() + _readerIndex; }
};

}

#endif // TINYSERVER_BUFFER_H
