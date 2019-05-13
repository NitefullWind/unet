set(unittest_SRCS
	unitTest_buffer
	unitTest_httpRequest
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
