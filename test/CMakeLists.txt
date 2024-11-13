include_directories(${CMAKE_SOURCE_DIR}/unix_stu)

add_executable(unix_stu_test 
    unix_stu_test.c
)

target_link_libraries(unix_stu_test PUBLIC
    unix_stu
)
