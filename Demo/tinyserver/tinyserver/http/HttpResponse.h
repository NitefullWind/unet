#ifndef TINYSERVER_HTTPRESPONSE_H
#define TINYSERVER_HTTPRESPONSE_H

namespace tinyserver
{
namespace http
{

class HttpResponse
{
public:
	explicit HttpResponse();
	~HttpResponse();

	void setKeeyAlive(bool KA) { _keeyAlive = KA; }
	bool keeyAlive() const { return _keeyAlive; }

private:
	bool _keeyAlive;
};

}
}

#endif // TINYSERVER_HTTPRESPONSE_H