#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;
using namespace tinyserver::http;

HttpRequest::HttpRequest()
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::setMethod(const char* methodString)
{
	if(strcasecmp(methodString, "GET") == 0) {
		_method = Get;
	} else if(strcasecmp(methodString, "POST") == 0) {
		_method = Post;
	} else if(strcasecmp(methodString, "PUT") == 0) {
		_method = Put;
	} else if(strcasecmp(methodString, "DELETE") == 0) {
		_method = Delete;
	} else {
		_method = Invalid;
	}
}

std::string HttpRequest::methodString() const
{
	switch (_method)
	{
	case Get:
		return "GET";
	case Post:
		return "POST";
	case Put:
		return "PUT";
	case Delete:
		return "DELETE";
	default:
		return "INVALID";
	}
}

void HttpRequest::setVersion(const char* versionString)
{
	if(strcasecmp(versionString, "HTTP/1.0") == 0) {
		_version = Http10;
	} else if(strcasecmp(versionString, "HTTP/1.1") == 0) {
		_version = Http11;
	} else {
		_version = Unknown;
	}
}

std::string HttpRequest::versionString() const
{
	switch (_version)
	{
	case Http10:
		return "HTTP/1.0";
	case Http11:
		return "HTTP/1.1";
	default:
		return "UNKNOWN";
	}
}

void HttpRequest::parserRequest(Buffer* buffer)
{
	std::string line = buffer->readLine(false);
	std::string::size_type n1 = line.find(' ');
	if(n1 != std::string::npos) {
		setMethod(line.substr(0, n1).c_str());
		std::string::size_type n2 = line.find(' ', n1+1);
		if(n2 != std::string::npos) {
			setPath(line.substr(n1+1, n2-n1-1).c_str());
			setVersion(line.substr(n2+1).c_str());
			return;
		}
	}
	// TODO: invalid request
}