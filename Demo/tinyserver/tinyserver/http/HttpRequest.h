#ifndef TINYSERVER_HTTPREQUEST_H
#define TINYSERVER_HTTPREQUEST_H

#include <string>
#include <map>

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

	/**
	 * 请求方法
	 */
	void setMethod(const char* methodString);
	Method method() const { return _method; }
	std::string methodString() const;

	/**
	 * HTTP版本
	 */
	void setVersion(const char* versionString);
	Version version() const { return _version; }
	std::string versionString() const;

	/**
	 * 请求路径
	 */
	void setPath(const std::string &path) { _path = path; }
	const std::string &path() const { return _path; }

	/**
	 * 返回请求中的url参数
	 */
	const std::map<std::string, std::string> &queries() const { return _queries; }
	std::string query(const std::string &field) const {
		try {
			return _queries.at(field);
		} catch(const std::exception& e) {
			return "";
		}
	}
	std::string query(std::string &&field) const {
		try {
			return _queries.at(std::move(field));
		} catch(const std::exception &e) {
			return "";
		}
	}

	/**
	 * 返回请求头部
	 */
	const std::map<std::string, std::string> &headers() const { return _headers; }
	std::string header(const std::string &field) const {
		try {
			return _headers.at(field);
		} catch(const std::exception& e) {
			return "";
		}
	}
	std::string header(std::string &&field) const {
		try {
			return _headers.at(std::move(field));
		} catch(const std::exception &e) {
			return "";
		}
	}

	const std::string &body() const { return _body; }

	/**
	 * 解析请求数据
	 */
	bool parserRequest(Buffer* buffer);

private:
	Method _method;
	Version _version;
	std::string _path;
	std::map<std::string, std::string> _queries;
	std::map<std::string, std::string> _headers;
	std::string _body;

	void parserURL(std::string &url);
	
};

}
}

#endif // TINYSERVER_HTTPREQUEST_H