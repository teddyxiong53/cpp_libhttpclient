cmake_minimum_required(VERSION 2.8)

include_directories("../include")
add_library(mod1 STATIC CurlEasyHandleWrapper.cpp libcurlUtils.cpp)
