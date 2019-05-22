cmake_minimum_required(VERSION 2.8)
add_definitions("-Wall -std=c++11 -DDEBUG=1 -g")
add_subdirectory(mod1)
add_subdirectory(mod2)
include_directories("./mod1/include")
include_directories("./mod2/include")
include_directories("./include")
add_executable(test
			main.cpp
			)
target_link_libraries(test mod1 mod2 pthread curl)
