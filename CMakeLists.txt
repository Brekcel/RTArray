
cmake_minimum_required(VERSION 3.0)
project(RTArray)
add_executable(TEST test.cpp)
set_property(TARGET TEST PROPERTY CXX_STANDARD 14)
set_property(TARGET TEST PROPERTY CXX_STANDARD_REQUIRED ON)
