set(base_SRC
	logger.cpp
	timestamp.cpp
	)

add_library(muduo_base ${base_SRC})
target_link_libraries(muduo_base log4cxx)
