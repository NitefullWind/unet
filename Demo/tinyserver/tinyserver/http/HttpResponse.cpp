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

HttpResponse::HttpResponse(HttpResponse &rhs)
{
	_statusCode = rhs._statusCode;
	_statusMessage = rhs._statusMessage;
	_headers = rhs._headers;
	_keepAlive = rhs._keepAlive;
	_textBody = rhs._textBody;
}

HttpResponse::HttpResponse(HttpResponse &&rhs)
{
	_statusCode = rhs._statusCode;
	_statusMessage = std::move(rhs._statusMessage);
	_headers = std::move(rhs._headers);
	_keepAlive = rhs._keepAlive;
	_textBody = std::move(rhs._textBody);
}

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs)
{
	if(this != &rhs) {
		_statusCode = rhs._statusCode;
		_statusMessage = rhs._statusMessage;
		_headers = rhs._headers;
		_keepAlive = rhs._keepAlive;
		_textBody = rhs._textBody;
	}
	return *this;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &&rhs)
{
	if(this != &rhs) {
		_statusCode = rhs._statusCode;
		_statusMessage = std::move(rhs._statusMessage);
		_headers = std::move(rhs._headers);
		_keepAlive = rhs._keepAlive;
		_textBody = std::move(rhs._textBody);
	}
	return *this;
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