#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>

#include <assert.h>
#include <sys/uio.h>

using namespace tinyserver;

Buffer::Buffer() :
	_buffer(kInitialSize),
	_writerIndex(0),
	_readerIndex(0)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *buf, size_t len)
{
	if(writeableBytes() < len) {
		_buffer.resize(_writerIndex + len);
	}
	std::copy(buf, buf+len, beginWrite());
	_writerIndex += len;
}

void Buffer::append(const std::string& str)
{
	append(str.data(), str.length());
}

ssize_t Buffer::readFd(int fd, int *savedErrno)
{
	struct iovec vec[2];
	const size_t writeable = writeableBytes();
	vec[0].iov_base = beginWrite();
	vec[0].iov_len = writeable;

	char extrabuf[65536];								// 64k extra buffer
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);

	ssize_t n = ::readv(fd, vec, 2);
	if(n < 0) {
		LOG_ERROR("readv errno = " << errno);
		*savedErrno = errno;
	} else if ((size_t)n < writeable) {
		_writerIndex += n;
	} else {
		_writerIndex = _buffer.size();
		append(extrabuf, n - writeable);
	}
	return n;
}

std::string Buffer::read(size_t len)
{
	assert(len <= readableBytes());
	std::string str(peek(), len);
	retrieve(len);
	return str;
}

std::string Buffer::readAll()
{
	std::string str(peek(), readableBytes());
	retrieveAll();
	return str;
}

std::string Buffer::readLine()
{
	const char* eol = findEOL();
	if(eol) {
		return read(eol - peek() + 1);
	}
	// 没有换行符，读取所有
	return readAll();
}

const char* Buffer::findCRLF() const
{
	return findCRLF(peek());
}

const char* Buffer::findCRLF(const char* start) const
{
	assert(peek() <= start);
	assert(start <= beginWrite());
	// 从指定位置开始查找\n
	const char* crlf = (const char*)memchr(start, '\n', beginWrite() - start);
	if(crlf && start < crlf && memcmp(crlf-1, "\r", 1) == 0) { // 查找\n前是否有\r
		return crlf - 1;
	}
	return crlf;
}

const char* Buffer::findEOL() const
{
	return findEOL(peek());
}

const char* Buffer::findEOL(const char* start) const
{
	assert(peek() <= start);
	assert(start <= beginWrite());
	// 从指定位置开始查找\n
	const char* crlf = (const char*)memchr(start, '\n', beginWrite() - start);
	return crlf;
}

void Buffer::retrieve(size_t len)
{
	assert(len <= readableBytes());
	if(len < readableBytes()) {
		_readerIndex += len;
	} else {
		retrieveAll();
	}
}

void Buffer::retrieveAll()
{
	_readerIndex = 0;
	_writerIndex = 0;
}