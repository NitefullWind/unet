#ifndef TINYSERVER_HTTPRESPONSE_H
#define TINYSERVER_HTTPRESPONSE_H

#include <tinyserver/buffer.h>
#include <map>
#include <string>

namespace tinyserver
{

namespace http
{

class HttpResponse
{
public:
	explicit HttpResponse(unsigned short statusCode=200, const std::string &statusMessage="OK");
	HttpResponse(HttpResponse &rhs);
	HttpResponse(HttpResponse &&rhs);
	HttpResponse &operator=(const HttpResponse &rhs);
	HttpResponse &operator=(const HttpResponse &&rhs);
	~HttpResponse();

	void setStatusCode(unsigned short code) { _statusCode = code; }
	unsigned short statusCode() const { return _statusCode; }
	void setStatusMessage(const std::string &message) { _statusMessage = message; }
	void setStatusMessage(std::string &&message) { _statusMessage = std::move(message); }
	const std::string &statusMessage() const { return _statusMessage; }

	/**
	 * 添加响应头
	 */
	void setHeader(const std::string &key, const std::string &value) { _headers[key] = value; }
	/**
	 * 返回响应头部
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
	void setContentType(const std::string &contentType) { setHeader("Content-Type", contentType); }
	void setKeepAlive(bool keep) {
		_keepAlive = keep;
		keep ? setHeader("Connection", "Keep-Alive") : setHeader("Connection", "close");
	}
	bool keepAlive() const { return _keepAlive; }

	void setTextBody(const std::string &text) { setResponseData(text.c_str(), text.length()); }
	void setTextBody(std::string &&text) { setResponseData(text.c_str(), text.length()); }
	std::string textBody() { return _rspBuffer.readAll(false); }

	void setResponseData(const void* data, size_t len) {
		_rspBuffer.retrieveAll();
		_rspBuffer.append(data, len);
		setHeader("Content-Length", std::to_string(len)); 
	}
	void getResponseBuffer(Buffer* buffer);
private:
	unsigned short _statusCode;
	std::string _statusMessage;
	std::map<std::string, std::string> _headers;
	bool _keepAlive;

	Buffer _rspBuffer;
};

}
}

#endif // TINYSERVER_HTTPRESPONSE_H