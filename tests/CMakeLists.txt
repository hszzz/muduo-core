add_executable(logger_test logger_test.cc)
target_link_libraries(logger_test muduo_core)

add_executable(timestamp_test timestamp_test.cc)
target_link_libraries(timestamp_test muduo_core)

add_executable(inet_address_test inet_address_test.cc)
target_link_libraries(inet_address_test muduo_core)

add_executable(current_thread_test current_thread_test.cc)
target_link_libraries(current_thread_test muduo_core pthread)

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test muduo_core)

add_executable(buffer_test buffer_test.cc)
target_link_libraries(buffer_test muduo_core)

