#include "client.h"
#include "server.h"
#include <thread>
#include <unistd.h>

int main() {
    Server server;
    std::thread serverThread([&server]() {
        server.start();
    });

    sleep(1); // 确保服务器先启动

    Client client;
    client.sendMessage("hello");

    serverThread.join();
    return 0;
}