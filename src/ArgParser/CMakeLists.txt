add_library(argparser STATIC ArgParser.cpp)

target_include_directories(
    argparser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)
