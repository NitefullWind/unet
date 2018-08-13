#include <muduo/net/buffer.h>

#include <errno.h>
#include <memory.h>
#include <sys/uio.h>

using namespace muduo;
using namespace muduo::net;

ssize_t Buffer::readFd(int fd, int *savedErrno)
{
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writable = writeableBytes();
	vec[0].iov_base = begin()+_writerIndex;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);
	const ssize_t n = readv(fd, vec, 2);
	if(n < 0) {
		*savedErrno = errno;
	} else if ((size_t)(n) <= writable) {
		_writerIndex += n;
	} else {
		_writerIndex = _buffer.size();
		append(extrabuf, n - writable);
	}
	return n;
}
