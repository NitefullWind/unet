include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC tcpservselect01.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(tcpservselect01 ${APP_SRC})

target_link_libraries(tcpservselect01 unp)

