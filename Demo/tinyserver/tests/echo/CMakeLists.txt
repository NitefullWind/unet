
add_executable(echo_server
	echo_server.cpp
	${tinyserver_SRCS}
	)

target_link_libraries(echo_server
	pthread
	)

if(${LOGLIB} MATCHES "log4cxx")
	target_link_libraries(echo_server
		log4cxx
		)
endif()