#ifndef TINYSERVER_BUFFER_H
#define TINYSERVER_BUFFER_H

#include <string>
#include <vector>

namespace tinyserver
{
/**
 * begin()			peek(),data()		beginWrite()
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
	Buffer(size_t size, size_t prependSize = 0);
	~Buffer();

	Buffer& operator=(const Buffer& rhs);

	// size_t size() const { return _buffer.size(); }	// useless
	size_t readableBytes() const { return _writerIndex - _readerIndex; }
	size_t writeableBytes() const { return _buffer.size() - _writerIndex; }
	size_t capacity() const { return _buffer.capacity(); }

	void setPrependSize(size_t prependSize) { _prependSize = prependSize; }
	size_t prependSize() const { return _prependSize; }

	void append(const char *buf, size_t len);
	void append(const void *buf, size_t len);
	void append(const std::string& str);
	void appendUInt64(uint64_t num);
	void appendUInt32(uint32_t num);
	void appendUInt16(uint16_t num);
	void appendUInt8(uint8_t num);
	ssize_t readFd(int fd, int *savedErrno);

	void prepend(const char *buf, size_t len);
	void prepend(const void *buf, size_t len);
	void prepend(const std::string& str);
	void prependUInt64(uint64_t num);
	void prependUInt32(uint32_t num);
	void prependUInt16(uint16_t num);
	void prependUInt8(uint8_t num);

	// read data to string
	std::string read(size_t len, bool needRetrieve=true);
	// read all data to string
	std::string readAll(bool needRetrieve=true);
	/**
	 * read a line to string
	 * @withCRLF if true return whith CRLF, default false.
	 */
	std::string readLine(bool withCRLF=true, bool needRetrieve=true);
	uint64_t readUInt64(bool needRetrieve=true);
	uint32_t readUInt32(bool needRetrieve=true);
	uint16_t readUInt16(bool needRetrieve=true);
	uint8_t readUInt8(bool needRetrieve=true);

	// return the string starting with \n or \r\n
	const char* findCRLF() const;
	const char* findCRLF(const char* start) const;

	// return the string starting with \n
	const char* findEOL() const;
	const char* findEOL(const char* start) const;

	// first char of the buffer that can read
	const char *peek() const { return begin() + _readerIndex; }
	char *data() { return begin() + _readerIndex; }
	const char *data() const { return begin() + _readerIndex; }

	// retrieve the buffer
	void retrieve(size_t len);
	// retrieve all the buffer space
	void retrieveAll();
private:
	std::vector<char> _buffer;
	size_t _prependSize;
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
