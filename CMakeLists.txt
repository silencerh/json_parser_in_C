cmake_minimum_required(VERSION 3.0)
project(json_c_parser)

add_library(mlnjson mlnjson.c)
add_executable(mlnjson_test test.c)
target_link_libraries(mlnjson_test mlnjson)
