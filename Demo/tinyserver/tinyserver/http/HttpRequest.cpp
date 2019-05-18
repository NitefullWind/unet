#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;
using namespace tinyserver::http;

HttpRequest::HttpRequest():
	_method(Invalid),
	_version(Unknown),
	_path(),
	_body()
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
			std::string url = line.substr(n1+1, n2-n1-1);
			parserURL(std::move(url));
			setVersion(line.substr(n2+1).c_str());

			// parse headers
			while((line = buffer->readLine(false)) != "") {
				n1 = line.find(':');
				if(n1 != std::string::npos) {
					_headers[line.substr(0, n1)] = line.substr(n1+2);
				}
			}
			_body = buffer->readAll();
		}
	}
	// TODO: invalid request
}

void HttpRequest::parserURL(std::string &&url)
{
	std::string::size_type n1 = url.find('?');
	if(n1 == std::string::npos) {	// 没有url参数
		setPath(url);
	} else {
		setPath(url.substr(0, n1));
		std::string queryStr = url.substr(n1+1);
		
		std::string::size_type n2 =std::string::npos;
		do {
			n1 = queryStr.find('=');
			n2 = queryStr.find('&');
			_queries[queryStr.substr(0, n1)] = queryStr.substr(n1+1, n2-n1-1);
			queryStr.erase(0, (n2 == std::string::npos) ? n2 : n2+1); //删除已解析的参数
		} while(n2 != std::string::npos);
	}
}