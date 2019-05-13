#ifndef TINYSERVER_BUFFER_H
#define TINYSERVER_BUFFER_H

#include <string>
#include <vector>

namespace tinyserver
{
/**
 * begin()				peek()			beginWrite()
 * |					|					|
 * +--------------------+-------------------+---------------+
 * |	read bytes		|	readable bytes	|writable bytes	|
 * |					|	  (CONTENT)  	|				|
 * +--------------------+-------------------+---------------+
 * |					|					|				|
 * _buffer.data()	 _readIndex			_writerIndex	_buffer.size()
 */
class Buffer
{
public:
	static const size_t kInitialSize = 1024;
	Buffer();
	~Buffer();

	// size_t size() const { return _buffer.size(); }	// useless
	size_t readableBytes() const { return _writerIndex - _readerIndex; }
	size_t writeableBytes() const { return _buffer.size() - _writerIndex; }

	void append(const char *buf, size_t len);
	void append(const std::string& str);
	ssize_t readFd(int fd, int *savedErrno);

	// read data to string
	std::string read(size_t len);
	// read all data to string
	std::string readAll();
	/**
	 * read a line to string
	 * @withCRLF if true return whith CRLF, default false.
	 */
	std::string readLine(bool withCRLF=true);

	// return the string starting with \n or \r\n
	const char* findCRLF() const;
	const char* findCRLF(const char* start) const;

	// return the string starting with \n
	const char* findEOL() const;
	const char* findEOL(const char* start) const;

	// first char of the buffer that can read
	const char *peek() const { return begin() + _readerIndex; }

	// retrieve the buffer
	void retrieve(size_t len);
	// retrieve all the buffer space
	void retrieveAll();
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
};

}

#endif // TINYSERVER_BUFFER_H
