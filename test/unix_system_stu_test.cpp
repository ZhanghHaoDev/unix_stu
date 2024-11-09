#include "unix_system_stu.h"
#include "unix_io_stu.h"
#include <iostream>

int main(int argc, char *argv[]) {
    unix_io_stu io_test;
    io_test.demo_standard_io();
    io_test.demo_error_handling();
    io_test.demo_file_io();

    unix_system_stu sys_test;
    //sys_test.log_test();

    return 0;
}