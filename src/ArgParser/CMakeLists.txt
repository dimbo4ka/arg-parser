add_library(argparser STATIC ArgParser.hpp ArgParser.cpp)

target_include_directories(argparser PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(argparser PRIVATE argument)
