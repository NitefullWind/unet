project(ChatRoom)

cmake_minimum_required(VERSION 3.0)

include_directories(${PROJECT_SOURCE_DIR}/../../include/)
include_directories(/usr/local/log4cxx/include/)
link_directories(${PROJECT_SOURCE_DIR}/../../lib/)
link_directories(/usr/local/log4cxx/lib/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

file(GLOB_RECURSE all_c_files ${CMAKE_SOURCE_DIR}/src/*.c)				# 遍历所有.c文件
set_source_files_properties(${all_c_files} PROPERTIES LANGUAGE CXX)		# 设置C文件用CXX编译器编译

#file(GLOB_RECURSE all_cxx_files ${CMAKE_SOURCE_DIR}/src/*.cpp)

#set(APP_SRC serv07.c ../pr_cpu_time.c ../web_child.c ../../lib/wrappthread.c pthread07.c)
set(APP_SRC 
		src/main.cpp
		src/control.cpp
		src/connection.cpp
		)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)

add_executable(ChatRoom_server ${APP_SRC})

target_link_libraries(ChatRoom_server unp pthread log4cxx)
