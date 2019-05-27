#include <tinyserver/http/HttpResponse.h>
#include <tinyserver/buffer.h>

using namespace tinyserver;
using namespace tinyserver::http;

HttpResponse::HttpResponse():
	_statusCode(0),
	_statusMessage(),
	_keepAlive(false),
	_textBody()
{
}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::getResponseBuffer(Buffer* buffer)
{
	buffer->append("HTTP/1.1 ");
	buffer->append(std::to_string(_statusCode));
	buffer->append(" ");
	buffer->append(_statusMessage);
	buffer->append("\r\n");
	for (auto &&it : _headers)
	{
		buffer->append(it.first);
		buffer->append(": ");
		buffer->append(it.second);
		buffer->append("\r\n");
	}
	buffer->append("\r\n");
	buffer->append(_textBody);
}