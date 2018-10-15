set(SRCS
	eventLoop.cpp
	logger.cpp
	)

add_library(tinyserver ${SRCS})

target_link_libraries(tinyserver
	log4cxx)
