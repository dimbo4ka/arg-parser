add_library(argparser ArgParser.cpp)

target_include_directories(
    argparser PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
)
