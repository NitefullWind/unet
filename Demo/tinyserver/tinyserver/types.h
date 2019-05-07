#ifndef TINYSERVER_TYPES_H
#define TINYSERVER_TYPES_H

#include <functional>
#include <memory>

namespace tinyserver
{

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer *buf)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

namespace http
{
class HttpRequest;
class HttpResponse;

typedef std::function<void(const HttpRequest&, const HttpResponse&)> HttpCallback;
}

}

#endif // TINYSERVER_TYPES_H
