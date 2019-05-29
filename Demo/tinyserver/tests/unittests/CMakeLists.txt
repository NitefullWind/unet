set(unittest_SRCS
	unitTest_buffer
	unitTest_httpRequest
	unitTest_httpResponse
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
