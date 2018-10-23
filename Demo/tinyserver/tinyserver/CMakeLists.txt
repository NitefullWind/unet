set(SRCS
	buffer.cpp
	channel.cpp
	tcpConnection.cpp
	eventLoop.cpp
	inetAddress.cpp
	logger.cpp
	poller.cpp
	sockets.cpp
	tcpServer.cpp
	)

add_library(tinyserver ${SRCS})

target_link_libraries(tinyserver
	log4cxx)
