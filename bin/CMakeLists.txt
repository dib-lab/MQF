cmake_minimum_required(VERSION 3.2)
project(MQF_main)


set(SOURCE_FILES main.cpp)

add_executable(MQF_main ${SOURCE_FILES})
target_link_libraries(MQF_main MQF)
