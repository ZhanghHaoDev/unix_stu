cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(unix_stu)

add_subdirectory(unix_system_stu)
add_subdirectory(unix_network_stu)
add_subdirectory(unix_network_helloword)
add_subdirectory(test)