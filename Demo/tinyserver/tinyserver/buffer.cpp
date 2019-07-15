#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <assert.h>
#include <sys/uio.h>

using namespace tinyserver;

Buffer::Buffer() :
	_buffer(kInitialSize),
	_writerIndex(0),
	_readerIndex(0)
{
}

Buffer::Buffer(size_t size) :
	_buffer(size),
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

void Buffer::append(const void *buf, size_t len)
{
	append(static_cast<const char *>(buf), len);
}

void Buffer::append(const std::string& str)
{
	append(str.data(), str.length());
}


void Buffer::appendInt64(int64_t num)
{
	auto be64 = sockets::HostToNetwork64(num);
	append(&be64, sizeof(be64));
}

void Buffer::appendInt32(int32_t num)
{
	auto be32 = sockets::HostToNetwork32(num);
	append(&be32, sizeof(be32));
}

void Buffer::appendInt16(int16_t num)
{
	auto be16 = sockets::HostToNetwork16(num);
	append(&be16, sizeof(be16));
}

void Buffer::appendInt8(int8_t num)
{
	append(&num, sizeof(num));
}

void Buffer::prepend(const char *buf, size_t len)
{
	if(len <= _readerIndex) {
		_readerIndex -= len;
		std::copy(buf, buf+len, begin() + _readerIndex);
	} else {
		if(writeableBytes() < len) {
			_buffer.resize(_writerIndex + len);
		}

		::memcpy(data()+len, data(), readableBytes()+len);
		_writerIndex += len;
		std::copy(buf, buf+len, data());
	}
}

void Buffer::prepend(const void *buf, size_t len)
{
	prepend(static_cast<const char*>(buf), len);
}

void Buffer::prepend(const std::string& str)
{
	prepend(str.c_str(), str.length());
}

void Buffer::prependInt64(int64_t num)
{
	int64_t be64 = sockets::HostToNetwork64(num);
	prepend(&be64, sizeof be64);
}

void Buffer::prependInt32(int32_t num)
{
	int32_t be32 = sockets::HostToNetwork32(num);
	prepend(&be32, sizeof be32);
}

void Buffer::prependInt16(int16_t num)
{
	int16_t be16 = sockets::HostToNetwork16(num);
	prepend(&be16, sizeof be16);
}

void Buffer::prependInt8(int8_t num)
{
	prepend(&num, sizeof num);
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
		TLOG_ERROR("readv errno = " << errno);
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

std::string Buffer::readLine(bool withCRLF)
{
	const char* eol = findEOL();
	if(eol) {
		std::string line = read(eol - peek() + 1);
		if(!withCRLF) {
			line.erase(line.length() - 1);
			if(line[line.length() - 1] == '\r') {
				line.erase(line.length() - 1);
			}
		}
		return line;
	}
	// 没有换行符，读取所有
	return readAll();
}


int64_t Buffer::readInt64()
{
	assert(readableBytes() >= sizeof(int64_t));
	int64_t be64 = 0;
	::memcpy(&be64, peek(), sizeof(be64));
	be64 = sockets::NetworkToHost64(be64);
	retrieve(sizeof(be64));
	return be64;
}
int32_t Buffer::readInt32()
{
	assert(readableBytes() >= sizeof(int32_t));
	int32_t be32 = 0;
	::memcpy(&be32, peek(), sizeof(be32));
	be32 = sockets::NetworkToHost32(be32);
	retrieve(sizeof(be32));
	return be32;
}
int16_t Buffer::readInt16()
{
	assert(readableBytes() >= sizeof(int16_t));
	int16_t be16 = 0;
	::memcpy(&be16, peek(), sizeof(be16));
	be16 = sockets::NetworkToHost16(be16);
	retrieve(sizeof(be16));
	return be16;
}
int8_t Buffer::readInt8()
{
	assert(readableBytes() >= sizeof(int8_t));
	int8_t be8 = 0;
	::memcpy(&be8, peek(), sizeof(be8));
	retrieve(sizeof(be8));
	return be8;
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