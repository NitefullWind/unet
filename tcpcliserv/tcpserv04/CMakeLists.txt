include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC tcpserv04.c sigchldwaitpid.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(tcpserv04 ${APP_SRC})

target_link_libraries(tcpserv04 unp)

