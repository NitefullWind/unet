
add_executable(http_server
	http_server.cpp
	${tinyserver_SRCS}
	)

target_link_libraries(http_server
	pthread
	log4cxx
	)