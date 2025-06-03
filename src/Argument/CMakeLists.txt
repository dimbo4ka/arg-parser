add_library(argument STATIC Argument.hpp Argument.cpp)

target_include_directories(argument PUBLIC ${CMAKE_SOURCE_DIR})
