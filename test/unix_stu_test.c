#include "unix_stu.h"

#include <pthread.h>
#include <unistd.h>

void *server_thread(void *arg) {
    server();
    return NULL;
}

void *client_thread(void *arg) {
    sleep(1); // 确保服务器先启动
    client();
    return NULL;
}

int main(int argc, char *argv[]) {
    demo_standard_io();
    demo_standard_io();
    demo_error_handling();
    demo_file_io();
    demo_file_system();

    pthread_t server_tid, client_tid;

    // 创建服务器线程
    pthread_create(&server_tid, NULL, server_thread, NULL);

    // 创建客户端线程
    pthread_create(&client_tid, NULL, client_thread, NULL);

    // 等待线程完成
    pthread_join(server_tid, NULL);
    pthread_join(client_tid, NULL);

    return 0;
}