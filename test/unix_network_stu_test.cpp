#include "unix_network_stu.h"
#include <thread>

int main() {
    unix_network_stu test;

    // 创建服务器线程
    std::thread server_thread([&test]() {
        test.server_socket();
    });

    // 确保服务器先启动
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 创建客户端线程
    std::thread client_thread([&test]() {
        test.client_socket();
    });

    // 等待线程完成
    server_thread.join();
    client_thread.join();

    return 0;
}