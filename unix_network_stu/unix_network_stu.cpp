#include "unix_network_stu.h"

#include <stdio.h>

unix_network_stu::unix_network_stu() = default;
unix_network_stu::~unix_network_stu() = default;

void unix_network_stu::check_endianness(){
    unsigned int x = 1;
    char *c = (char*)&x;
    if (*c) {
        printf("低字节序\n");
    } else {
        printf("高字节序\n");
    }
}