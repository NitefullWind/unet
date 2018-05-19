project(UnixNetwork)

add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(GetTime)
add_subdirectory(tcpcliserv)
add_subdirectory(sockopt)
add_subdirectory(udpcliserv)

set(APP_SRC main.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(main ${APP_SRC})
