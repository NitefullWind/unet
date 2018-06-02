include_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_BINARY_DIR}/lib)

set(APP_SRC daytimetcpcli.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(daytimetcpcli ${APP_SRC})

target_link_libraries(daytimetcpcli unp)

