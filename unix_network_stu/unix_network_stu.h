#ifndef UNIX_NETWORK_STU_H
#define UNIX_NETWORK_STU_H

class unix_network_stu{
public:
    unix_network_stu();
    ~unix_network_stu();

    // 检查机器的字节序
    void check_endianness();

    // 客户端使用套接字编程的过程
    int client_socket();

    // 服务端使用套接字编程的过程
    int server_socket();

};

#endif // UNIX_NETWORK_STU_H