set (base_SRCS
  test.cc
)

add_library(muduo_base ${base_SRCS})
target_link_libraries(muduo_base pthread)

add_subdirectory(tests)
