include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC serv07.c ../pr_cpu_time.c ../web_child.c ../../lib/wrappthread.c pthread07.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/server)

add_executable(serv07 ${APP_SRC})

target_link_libraries(serv07 unp pthread)
