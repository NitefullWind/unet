set(net_SRCS
		eventLoop.cpp
		channel.cpp
		poller.cpp
		timerQueue.cpp
		timer.cpp
		eventLoopThread.cpp
		socket.cpp
		socketsOps.cpp
		inetAddress.cpp
		acceptor.cpp
		tcpServer.cpp
		tcpConnection.cpp
		buffer.cpp
		eventLoopThreadPool.cpp
		connector.cpp
		)

add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base log4cxx pthread)

install(TARGETS muduo_net DESTINATION lib)

set(HEADERS
		eventLoop.h
		channel.h
		poller.h
		timerId.h
		eventLoopThread.h
		tcpServer.h
		buffer.h
		eventLoopThreadPool.h
		)

install(FILES ${HEADERS} DESTINATION include/muduo/net)
