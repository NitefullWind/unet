project(ChatRoom)

cmake_minimum_required(VERSION 3.0)

include_directories(${PROJECT_SOURCE_DIR}/../../include/)
link_directories(${PROJECT_SOURCE_DIR}/../../lib/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

#set(APP_SRC serv07.c ../pr_cpu_time.c ../web_child.c ../../lib/wrappthread.c pthread07.c)
set(APP_SRC src/main.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)

add_executable(ChatRoom_server ${APP_SRC})

target_link_libraries(ChatRoom_server unp)
