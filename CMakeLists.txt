cmake_minimum_required(VERSION 3.12)

project(
    argument_parser
    VERSION 1.0
    DESCRIPTION "cli-argparser"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(bin)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
