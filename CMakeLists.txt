cmake_minimum_required(VERSION 3.13)
project(static_reflection)

set(CMAKE_CXX_STANDARD 14)

add_executable(static_reflection main.cpp)
target_link_libraries(static_reflection jsoncpp)