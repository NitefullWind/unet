include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC
		web.c 
		home_page.c 
		start_connect.c 
		write_get_cmd.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(web ${APP_SRC})

target_link_libraries(web unp)

