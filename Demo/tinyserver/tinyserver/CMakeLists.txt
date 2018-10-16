set(SRCS
	eventLoop.cpp
	inetAddress.cpp
	logger.cpp
	sockets.cpp
	)

add_library(tinyserver ${SRCS})

target_link_libraries(tinyserver
	log4cxx)
