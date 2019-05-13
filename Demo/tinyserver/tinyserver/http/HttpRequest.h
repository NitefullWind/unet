#ifndef TINYSERVER_HTTPREQUEST_H
#define TINYSERVER_HTTPREQUEST_H

#include <string>

namespace tinyserver
{

class Buffer;

namespace http
{

class HttpRequest
{
public:

	enum Method
	{
		Invalid, Get, Post, Put, Delete
	};

	enum Version
	{
		Unknown, Http10, Http11
	};

	explicit HttpRequest();
	~HttpRequest();

	void setMethod(const char* methodString);
	Method method() const { return _method; }
	std::string methodString() const;

	void setVersion(const char* versionString);
	Version version() const { return _version; }
	std::string versionString() const;

	void setPath(const std::string &path) { _path = path; }
	std::string path() const { return _path; }

	void parserRequest(Buffer* buffer);

private:
	Method _method;
	Version _version;
	std::string _path;
};

}
}

#endif // TINYSERVER_HTTPREQUEST_H