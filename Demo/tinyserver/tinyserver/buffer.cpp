#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <assert.h>
#include <sys/uio.h>

using namespace tinyserver;

Buffer::Buffer() :
	_buffer(kInitialSize),
	_prependSize(0),
	_writerIndex(_prependSize),
	_readerIndex(_prependSize)
{
}

Buffer::Buffer(size_t size, size_t prependSize) :
	_buffer(size),
	_prependSize(prependSize),
	_writerIndex(_prependSize),
	_readerIndex(_prependSize)
{
}

Buffer::~Buffer()
{
}

Buffer& Buffer::operator=(const Buffer& rhs)
{
	if(this != &rhs) {
		_buffer = rhs._buffer;
		_writerIndex = rhs._writerIndex;
		_readerIndex = rhs._readerIndex;
	}
	return *this;
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


void Buffer::appendUInt64(uint64_t num)
{
	auto be64 = sockets::HostToNetwork64(num);
	append(&be64, sizeof(be64));
}

void Buffer::appendUInt32(uint32_t num)
{
	auto be32 = sockets::HostToNetwork32(num);
	append(&be32, sizeof(be32));
}

void Buffer::appendUInt16(uint16_t num)
{
	auto be16 = sockets::HostToNetwork16(num);
	append(&be16, sizeof(be16));
}

void Buffer::appendUInt8(uint8_t num)
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

		// ::memcpy(data()+len, data(), readableBytes());
		//!NOTE 可能出现内存重叠，但dst > src，std::copy从低地址开始复制，函数行为正常。
		auto bufLen = readableBytes();
		if(bufLen > 0) {
			std::copy(peek(), peek()+bufLen, data()+len);
		}
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

void Buffer::prependUInt64(uint64_t num)
{
	uint64_t be64 = sockets::HostToNetwork64(num);
	prepend(&be64, sizeof be64);
}

void Buffer::prependUInt32(uint32_t num)
{
	uint32_t be32 = sockets::HostToNetwork32(num);
	prepend(&be32, sizeof be32);
}

void Buffer::prependUInt16(uint16_t num)
{
	uint16_t be16 = sockets::HostToNetwork16(num);
	prepend(&be16, sizeof be16);
}

void Buffer::prependUInt8(uint8_t num)
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
		_writerIndex += (size_t)n;
	} else {
		_writerIndex = _buffer.size();
		append(extrabuf, (size_t)n - writeable);
	}
	return n;
}

std::string Buffer::read(size_t len, bool needRetrieve)
{
	assert(len <= readableBytes());
	std::string str(peek(), len);
	if(needRetrieve) { retrieve(len); }
	return str;
}

std::string Buffer::readAll(bool needRetrieve)
{
	std::string str(peek(), readableBytes());
	if(needRetrieve) { retrieveAll(); }
	return str;
}

std::string Buffer::readLine(bool withCRLF, bool needRetrieve)
{
	const char* eol = findEOL();
	if(eol) {
		assert(eol - peek() + 1 > 0);
		std::string line = read((size_t)(eol - peek() + 1));
		if(!withCRLF) {
			line.erase(line.length() - 1);
			if(line[line.length() - 1] == '\r') {
				line.erase(line.length() - 1);
			}
		}
		return line;
	}
	// 没有换行符，读取所有
	return readAll(needRetrieve);
}


uint64_t Buffer::readUInt64(bool needRetrieve)
{
	assert(readableBytes() >= sizeof(uint64_t));
	uint64_t be64 = 0;
	::memcpy(&be64, peek(), sizeof(be64));
	be64 = sockets::NetworkToHost64(be64);
	if(needRetrieve) { retrieve(sizeof(be64)); }
	return be64;
}
uint32_t Buffer::readUInt32(bool needRetrieve)
{
	assert(readableBytes() >= sizeof(uint32_t));
	uint32_t be32 = 0;
	::memcpy(&be32, peek(), sizeof(be32));
	be32 = sockets::NetworkToHost32(be32);
	if(needRetrieve) { retrieve(sizeof(be32)); }
	return be32;
}
uint16_t Buffer::readUInt16(bool needRetrieve)
{
	assert(readableBytes() >= sizeof(uint16_t));
	uint16_t be16 = 0;
	::memcpy(&be16, peek(), sizeof(be16));
	be16 = sockets::NetworkToHost16(be16);
	if(needRetrieve) { retrieve(sizeof(be16)); }
	return be16;
}
uint8_t Buffer::readUInt8(bool needRetrieve)
{
	assert(readableBytes() >= sizeof(uint8_t));
	uint8_t be8 = 0;
	::memcpy(&be8, peek(), sizeof(be8));
	if(needRetrieve) { retrieve(sizeof(be8)); }
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
	const char* crlf = (const char*)memchr(start, '\n', (size_t)(beginWrite() - start));
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
	const char* crlf = (const char*)memchr(start, '\n', (size_t)(beginWrite() - start));
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
	_readerIndex = _writerIndex = _prependSize;
}