## 说明

此项目为本人在读陈硕大佬的《Linux多线程服务端编程:使用muduo C++ 网络库》时，仿照muduo库自己实现的一个版本。

muduo地址：https://github.com/chenshuo/muduo

### 与muduo差异

- 使用log4cxx日志库
- 使用C++11中的thread、mutex、unique_ptr、functional等

### 测试程序一览

- test1.cpp 测试EventLoop
- test2.cpp 测试Channel
- test3.cpp 测试EventLoop中的runAfter()和runEvery()
- test4.cpp 测试EventLoop中的runInLoop()和queueInLoop()
- test5.cpp 测试使用EventLoopThread创建事件循环
- test6.cpp 测试使用Acceptor监听新连接
- test7.cpp 测试使用TcpServer接收套接字数据
- test8.cpp 测试使用TcpServer收发数据
- test9.cpp 测试使用Connector创建连接
