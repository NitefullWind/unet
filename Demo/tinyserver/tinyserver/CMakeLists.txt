set(SRCS
	buffer.cpp
	channel.cpp
	eventLoop.cpp
	eventLoopThread.cpp
	eventLoopThreadPool.cpp
	inetAddress.cpp
	logger.cpp
	poller.cpp
	sockets.cpp
	tcp/tcpServer.cpp
	tcp/tcpConnection.cpp
	)

add_library(tinyserver ${SRCS})

target_link_libraries(tinyserver
	log4cxx)
