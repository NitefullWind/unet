set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/test/)

# for debug
set(muduo_net_SRC 
	../muduo/net/eventLoop.cpp
	../muduo/net/channel.cpp
	../muduo/net/poller.cpp
	../muduo/net/timerQueue.cpp
	../muduo/net/timer.cpp
	)

# for debug
#add_executable(test2 test2.cpp  ${muduo_net_SRC})
#target_link_libraries(test2 log4cxx muduo_base)

add_executable(test1 test1.cpp)
target_link_libraries(test1 muduo_net pthread)

add_executable(test2 test2.cpp)
target_link_libraries(test2 muduo_net)

add_executable(test3 test3.cpp)
target_link_libraries(test3 muduo_net)

add_executable(test4 test4.cpp)
target_link_libraries(test4 muduo_net)

add_executable(test5 test5.cpp)
target_link_libraries(test5 muduo_net pthread)

add_executable(test6 test6.cpp)
target_link_libraries(test6 muduo_net pthread)

add_executable(test7 test7.cpp)
target_link_libraries(test7 muduo_net pthread)
