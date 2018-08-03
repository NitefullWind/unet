set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/test/)

# for debug
set(muduo_net_SRC 
	../muduo/net/eventLoop.cpp
	../muduo/net/channel.cpp
	../muduo/net/poller.cpp
	)

# for debug
#add_executable(test2 test2.cpp  ${muduo_net_SRC})
#target_link_libraries(test2 log4cxx muduo_base)

add_executable(test1 test1.cpp)
target_link_libraries(test1 muduo_net pthread)

add_executable(test2 test2.cpp)
target_link_libraries(test2 muduo_net)
