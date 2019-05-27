#ifndef TINYSERVER_HTTPRESPONSE_H
#define TINYSERVER_HTTPRESPONSE_H

#include <map>
#include <string>

namespace tinyserver
{

class Buffer;

namespace http
{

class HttpResponse
{
public:
	explicit HttpResponse();
	~HttpResponse();

	void setStatusCode(unsigned short code) { _statusCode = code; }
	void setStatusMessage(const std::string &message) { _statusMessage = message; }
	void setStatusMessage(std::string &&message) { _statusMessage = std::move(message); }

	void setHeader(const std::string &key, const std::string &value) { _headers[key] = value; }
	void setContentType(const std::string &contentType) { setHeader("Content-Type", contentType); }
	void setKeepAlive(bool keep) {
		_keepAlive = keep;
		keep ? setHeader("Connection", "Keep-Alive") : setHeader("Connection", "close");
	}
	bool keepAlive() const { return _keepAlive; }

	void setTextBody(const std::string &text) { _textBody = text; }
	void setTextBody(std::string &&text) { _textBody = std::move(text); }

	void getResponseBuffer(Buffer* buffer);
private:
	unsigned short _statusCode;
	std::string _statusMessage;
	std::map<std::string, std::string> _headers;
	bool _keepAlive;

	std::string _textBody;
};

}
}

#endif // TINYSERVER_HTTPRESPONSE_H