#include "unix_system_stu.h"
#include "unix_io_stu.h"

int main(int argc, char *argv[]) {
    unix_io_stu io_test;
    io_test.demo_standard_io();
    io_test.demo_error_handling();


    return 0;
}