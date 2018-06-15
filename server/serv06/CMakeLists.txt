include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC serv06.c ../pr_cpu_time.c ../web_child.c ../../lib/wrappthread.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/server)

add_executable(serv06 ${APP_SRC})

target_link_libraries(serv06 unp pthread)
