set(base_SRC
	logger.cpp
	)

add_library(muduo_base ${base_SRC})
target_link_libraries(muduo_base log4cxx)
