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
	)

add_subdirectory(unittests)
add_subdirectory(echo)