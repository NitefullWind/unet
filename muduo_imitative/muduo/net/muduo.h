#include <functional>
#include <memory>

#include <muduo/base/timestamp.h>

namespace muduo
{
namespace net
{

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer *buf, Timestamp)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

}
}
