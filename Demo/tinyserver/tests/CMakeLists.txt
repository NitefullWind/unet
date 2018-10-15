set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/tests/)

set(tinyserver_SRCS
	../tinyserver/eventLoop.cpp
	../tinyserver/logger.cpp
	)

set(unittest_SRCS
	unitTest_eventLoop
	)

add_executable(unitTest
	unitTest.cpp
	${unittest_SRCS}
	${tinyserver_SRCS}
	)

target_link_libraries(unitTest
	gtest
	pthread
	log4cxx
	)
