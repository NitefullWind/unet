cmake_minimum_required(VERSION 2.6)

project(muduo_imitative CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "debug")
endif()

set(CXX_FLAGS
	-g
	# -DVALGRIND
	# -DMUDUO_STD_STRING
	-Wall
	-Wextra
	# -m32
	-Werror
	-Wconversion
	-Wno-unused-parameter
	#-Wold-style-cast
	#-Woverloaded-virtual
	-Wpointer-arith
	-Wshadow
	-Wwrite-strings
	-march=native
	# -MMD
	-std=c++11
	-rdynamic
	)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#find_path(log4cxx_INCLUDE_DIR log4cxx "/usr/local/log4cxx/include")
set(log4cxx_INCLUDE_DIR "/usr/local/log4cxx/include")
find_path(log4cxx_LIBRARY_DIR liblog4cxx.a "/usr/local/log4cxx/lib")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${log4cxx_LIBRARY_DIR})
link_directories(${log4cxx_LIBRARY_DIR})
message(STATUS ${log4cxx_INCLUDE_DIR})
message(STATUS ${log4cxx_LIBRARY_DIR})

include_directories(${log4cxx_INCLUDE_DIR})
find_library(log4cxx log4cxx)
message(STATUS ${log4cxx})

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(muduo/base)
add_subdirectory(muduo/net)

add_subdirectory(test)
