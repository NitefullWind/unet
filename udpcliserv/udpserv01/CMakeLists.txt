include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC udpserv01.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(udpserv01 ${APP_SRC})

target_link_libraries(udpserv01 unp)

