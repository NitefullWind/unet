
[TOC]

### 构建方式:
`BUILD_TYPE=debug LOGLIB=log4cxx BUILD_TEST=ON ./build.sh`
构建参数：
| 参数名 | 可选值 | 默认值 | 说明 |
|--|--|--|--|
| BUILD_DIR | 任意路径 | ./build | 构建生产临时文件路径 |
| BUILD_TYPE | debug,release | debug | debug或release版本 |
| INSTALL_DIR | 任意路径 | ${BUILD_DIR}/${BUILD_TYPE}-install | 安装版本生成目录 |
| LOGLIB | log4cxx,spdlog | 空 | 使用的日志库 |
| BUILD_TEST | ON,OFF | ON | 构建测试项目 |
| BUILD_UNITTEST | ON,OFF | OFF | 构建单元测试项目 |

### 切换使用poll或者epoll

在环境变量中设置TINYSERVER_USE_POLL=poll则使用poll, 否则默认使用epoll

### 问题记录：

#### 多线程时IO线程阻塞在poll()函数中而无法处理新连接
2018年10月27日14:32:50

当前版本([894ba8c](https://github.com/NitefullWind/unet/commit/894ba8c6e7b2d41f3ae7180a511678872904dfd6))有一个很容易重现的问题，当TcpServer有单独的IO线程时，当主EventLoop线程接收到新的连接，会将新连接分发给IO线程，而IO线程此时很有可能正阻塞在poll()系统调用中，当然它也不会知道有新的socket需要加入pollfds进行监听。此时，如果新连接的socket有任何事件发生，系统都不会知道，只有等当前poll()返回，EventLoop再次调用poll()的时候，才会检测到新连接上发生的事件。如果poll()的超时时间设置为永远等待，那将永远也不能处理新连接的任何事件。解决办法是，在EventLoop添加一个特殊的描述符(eventfd)，并添加到poll()中进行监听它的可读事件，当需要唤起poll()的阻塞时，只需向它写入数据即可。

已在版本：[8a55d28](https://github.com/NitefullWind/unet/commit/8a55d28c511477b6f7b1ccd50fbc0483291aa3a0) 解决。

#### 多线程时关闭连接导致主线程报错不能接受新连接
2019年2月27日15:54:30

之前版本([25ced31](https://github.com/NitefullWind/unet/commit/25ced31d087a9071bfd67d47af7dabd8ef2038bf))在多线程时，客户端关闭Socket后可能导致服务器不能接受新的连接(Issue:[#2](https://github.com/NitefullWind/unet/issues/2))

调试发现，IO线程接收到关闭连接请求后，回调Server线程中的删除TCP连接函数，而IO线程中的poll()函数可能再次返回，pollfd没被重置，IO线程再次执行关闭连接操作且还会回调Server线程中的删除TCP连接函数。

而`TcpServer::removeConnectionInLoop(const TcpConnectionPtr& tcpConnPtr)`函数中，没有检查连接是否存在，直接执行删除操作，Server线程执行`_connectionMap.erase()`时出错，Server线程停止运行，虽然IO线程仍在继续执行，但Server线程中的监听工作已不能正常进行，服务器不能接受新的连接。