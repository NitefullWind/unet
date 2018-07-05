project(unplib)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

file(GLOB_RECURSE all_c_files ${CMAKE_SOURCE_DIR}/*.c)
set_source_files_properties(${all_c_files} PROPERTIES LANGUAGE CXX)

set(LIB_SRC 
		error.c 
		wrapsock.c 
		wrapunix.c 
		wrappthread.c
		str_echo.c
		#str_cli.c
		#str_cli11.c
		#../select/strcliselect01.c
		#../select/strcliselect02.c
		#../elect/strclielect02.c
		#../nonblock/strclinonb.c
		#../nonblock/strclifork.c
		readn.c
		writen.c
		wraplib.c
		wrapstdio.c
		readline.c
		signal.c

		#dg_echo.c
		#../sigio/dgecho01.c
		#dg_cli.c
		#../bcast/dgclibcast.c
		gf_time.c
		host_serv.c
		sock_ntop.c
		sock_ntop_host.c
		tcp_connect.c
		tcp_listen.c
		read_fd.c
		write_fd.c
	)
add_library(unp ${LIB_SRC})

target_link_libraries(unp pthread)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)

