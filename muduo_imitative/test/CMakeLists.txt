set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/test/)

add_executable(test1 test1.cpp)
target_link_libraries(test1 muduo_net pthread)
