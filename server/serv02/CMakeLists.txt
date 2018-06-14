include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC serv02.c child02.c ../pr_cpu_time.c ../web_child.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/server)

add_executable(serv02 ${APP_SRC})

target_link_libraries(serv02 unp)

