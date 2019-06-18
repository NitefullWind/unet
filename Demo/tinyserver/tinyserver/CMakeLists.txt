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
	http/HttpRequest.cpp
	http/HttpResponse.cpp
	http/HttpServer.cpp
	)

add_library(tinyserver ${SRCS})

if(${LOGLIB} MATCHES "log4cxx")
	target_link_libraries(tinyserver
		log4cxx
		)
endif()