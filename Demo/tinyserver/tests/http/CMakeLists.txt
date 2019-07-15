
add_executable(http_server
	http_server.cpp
	)

target_link_libraries(http_server
	tinyserver
	pthread
	)

if(${LOGLIB} MATCHES "log4cxx")
	target_link_libraries(http_server
		log4cxx
		)
endif()