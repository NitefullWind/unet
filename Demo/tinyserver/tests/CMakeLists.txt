set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/tests/)

set(tinyserver_SRCS
	 ${PROJECT_SOURCE_DIR}/tinyserver/buffer.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/channel.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/eventLoop.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/eventLoopThread.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/eventLoopThreadPool.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/inetAddress.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/logger.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/poller.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/sockets.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/tcp/tcpConnection.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/tcp/tcpServer.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/http/HttpRequest.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/http/HttpResponse.cpp
	 ${PROJECT_SOURCE_DIR}/tinyserver/http/HttpServer.cpp
	)

if(${BUILD_UNITTEST} MATCHES "ON")
	add_subdirectory(unittests)
endif()

add_subdirectory(echo)
add_subdirectory(http)