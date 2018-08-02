set(net_SRCS
		eventLoop.cpp
		)

add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base log4cxx)

install(TARGETS muduo_net DESTINATION lib)

set(HEADERS
		eventLoop.h
		)

install(FILES ${HEADERS} DESTINATION include/muduo/net)
