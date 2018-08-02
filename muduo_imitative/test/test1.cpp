#include <muduo/net/eventLoop.h>
#include <muduo/base/logger.h>
#include <thread>
#include <unistd.h> // getpid()

void threadFunc()
{
	LOG_TRACE(__FUNCTION__ << " pid = " << getpid() 
		<< ", tid = " << std::this_thread::get_id());
	muduo::net::EventLoop loop;
	loop.loop();
}

int main()
{
	muduo::Logger::InitByFile("log.props");
	LOG_TRACE("test trace");
	LOG_DEBUG("test debug");
	LOG_WARN("test warning");
	LOG_ERROR("test error");
//	LOG_FATAL("test fatal");			// 会终止程序

	auto loop = new muduo::net::EventLoop();
	loop->loop();

	delete loop;

	LOG_TRACE("delete the loop and create another one: ");
	loop = new muduo::net::EventLoop();

	LOG_TRACE("create another loop in other thread:");
	std::thread thread(threadFunc);
	thread.join();
	
	loop->loop();
	
	LOG_TRACE("don't delete the loop and create another one, will abort: ");
	muduo::net::EventLoop errorLoop;	// 这里会报错，因为上个loop还未析构，此线程不能再创建loop
	errorLoop.loop();

	return 0;
}
