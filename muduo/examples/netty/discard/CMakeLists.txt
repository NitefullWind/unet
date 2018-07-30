add_executable(netty_discard_client client.cc)
target_link_libraries(netty_discard_client ${muduo_net} ${muduo_base} pthread rt)

add_executable(netty_discard_server server.cc)
target_link_libraries(netty_discard_server ${muduo_net} ${muduo_base} pthread rt)
