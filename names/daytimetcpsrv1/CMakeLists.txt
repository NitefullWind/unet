include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC daytimetcpsrv1.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(daytimetcpsrv1 ${APP_SRC})

target_link_libraries(daytimetcpsrv1 unp)

