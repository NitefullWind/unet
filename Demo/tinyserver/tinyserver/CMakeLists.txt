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

install(TARGETS tinyserver DESTINATION lib)
set(HEADERS
	buffer.h
	channel.h
	eventLoop.h
	eventLoopThread.h
	eventLoopThreadPool.h
	inetAddress.h
	logger.h
	types.h
)
install(FILES ${HEADERS} DESTINATION include/tinyserver)

set(HEADERS_TCP
	tcp/tcpConnection.h
	tcp/tcpServer.h
)
install(FILES ${HEADERS_TCP} DESTINATION include/tinyserver/tcp)

set(HEADERS_HTTP
	http/HttpRequest.h
	http/HttpResponse.h
	http/HttpServer.h
)
install(FILES ${HEADERS_HTTP} DESTINATION include/tinyserver/http)