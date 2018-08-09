#include <functional>
#include <memory>

namespace muduo
{
namespace net
{

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, const char *data, ssize_t len)> MessageCallback;

}
}
