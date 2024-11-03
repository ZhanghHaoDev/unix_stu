include_directories(
    ${CMAKE_SOURCE_DIR}/unix_network_helloword
    ${CMAKE_SOURCE_DIR}/unix_network_stu
    ${CMAKE_SOURCE_DIR}/unix_system_stu
)

add_executable(unix_network_helloword_test 
    unix_network_helloword_test.cpp
)

target_link_libraries(unix_network_helloword_test PUBLIC
    unix_network_helloword
)

################################################################################

add_executable(unix_system_stu_test 
    unix_system_stu_test.cpp
)

target_link_libraries(unix_system_stu_test PUBLIC
    unix_system_stu
)

################################################################################

add_executable(unix_network_stu_test 
    unix_network_stu_test.cpp
)

target_link_libraries(unix_network_stu_test PUBLIC
    unix_network_stu
)

