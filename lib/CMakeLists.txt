set(LIB_SRC 
		error.c 
		wrapsock.c 
		wrapunix.c 
		str_echo.c
		#str_cli.c
		#str_cli11.c
		#../select/strcliselect01.c
		#../select/strcliselect02.c
		#../elect/strclielect02.c
		#../nonblock/strclinonb.c
		../nonblock/strclifork.c
		readn.c
		writen.c
		wraplib.c
		wrapstdio.c
		readline.c
		signal.c

		#dg_echo.c
		../sigio/dgecho01.c
		#dg_cli.c
		../bcast/dgclibcast.c
		gf_time.c
		host_serv.c
		sock_ntop.c
		sock_ntop_host.c
		tcp_connect.c
		tcp_listen.c
	)
add_library(unp ${LIB_SRC})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/lib)

