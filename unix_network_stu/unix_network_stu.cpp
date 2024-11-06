#include "unix_network_stu.h"

#include <cstdio>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <sys/_endian.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

int unix_network_stu::client_socket(){
    /*
    1. 创建套接字：使用 socket 函数创建一个新的套接字。
    2. 连接到服务器：使用 connect 函数将套接字连接到服务器的地址和端口。
    3. 发送和接收数据：使用 send 和 recv 函数进行数据传输。
    4. 关闭套接字：使用 close 函数关闭套接字
    */

    // 1. 创建套接字
    int socket_er = 0;
    if ((socket_er = socket(PF_INET,SOCK_STREAM,0)) == -1){
        perror("创建socket套接字失败\n");
        return socket_er;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8081);

    // 将服务器的 IP 地址转换为网络字节序并存储在 server_address.sin_addr.s_addr 中
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("无效的地址/地址不支持\n");
        close(socket_er);
        return -1;
    }
    
    // 2. 连接到服务器
    if(connect(socket_er,(struct sockaddr *)&server_address,sizeof(server_address)) == -1){
        perror("连接到服务器失败\n");
        close(socket_er);
        return -1;
    }

    printf("成功连接到服务器\n");

    // 3. 发送和接收数据
    const char *message = "Hello, Server!";
    ssize_t sedre = send(socket_er,message,strlen(message),0);
    if (sedre == -1){
        perror("发送数据失败\n");
        close(socket_er);
        return -1;
    }

    printf("成功发送数据\n");

    // 4. 接收服务器的回复
    char buffer[1024] = {0};
    if(recv(socket_er,buffer,sizeof(buffer),0) == -1){
        perror("接收服务器数据错误\n");
        close(socket_er);
        return -1;
    }

    // 5. 关闭套接字
    close(socket_er);

    return socket_er;
}

int unix_network_stu::server_socket(){
    /*
    1. 创建套接字：使用 socket 函数创建一个新的套接字。
    2. 绑定套接字：使用 bind 函数将套接字绑定到一个本地地址和端口。
    3. 监听连接：使用 listen 函数将套接字设置为被动模式，准备接受连接。
    4. 接受连接：使用 accept 函数接受一个连接请求，返回一个新的套接字用于通信。
    5. 发送和接收数据：使用 send 和 recv 函数进行数据传输。
    6. 关闭套接字：使用 close 函数关闭套接字。
    */

    int socket_er = 0;
    // 1. 创建套接字
    if ((socket_er = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("创建socket套接字失败\n");
        return socket_er;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8081);
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    // 2. 绑定套接字
    if (bind(socket_er, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("绑定套接字失败\n");
        close(socket_er);
        return -1;
    }

    // 3. 监听连接
    if(listen(socket_er, 5) == -1){
        perror("监听连接失败\n");
        close(socket_er);
        return -1;
    }

    // 4. 接受连接
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_socket = accept(socket_er, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket == -1){
        perror("接受连接错误\n");
        close(socket_er);
        return -1;
    }

    // 5. 发送和接收数据
    // 接收客户端发送的数据
    char buffer[1024] = {0};
    if(recv(client_socket,buffer,sizeof(buffer),0) == -1){
        perror("接收数据错误\n");
        close(client_socket);
        close(socket_er);
        return -1;
    }

    printf("从客户端接收到的数据 %s\n",buffer);

    // 向客户端发送回复
    const char* reply = "hello client";
    if(send(client_socket,reply,strlen(reply),0) == -1){
        perror("发送数据错误\n");
        close(socket_er);
        close(client_socket);
        return -1;
    }

    // 6. 关闭套接字
    close(socket_er);
    close(socket_er);
    
    return socket_er;
}