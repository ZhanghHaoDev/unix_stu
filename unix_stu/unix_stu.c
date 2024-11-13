#include "unix_stu.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/stat.h>
#include <arpa/inet.h>

#define PORT 8080

void demo_standard_io() {
    // 1. 使用 printf 格式化输出到标准输出
    int num = 42;
    printf("1. printf: 答案是 %d\n", num);

    // 2. 使用 perror 输出错误信息
    FILE *file = fopen("nonexistent.txt", "r");
    if (file == NULL) {
        perror("2. perror: 打开文件错误");
    }

    // 3. 使用 fprintf 格式化输出到指定文件流
    file = fopen("output.txt", "w");
    if (file != NULL) {
        fprintf(file, "3. fprintf: 你好，文件！\n");
        fclose(file);
    }

    // 4. 使用 sprintf 格式化输出到字符串
    char buffer[50];
    sprintf(buffer, "4. sprintf: 答案是 %d", num);
    printf("%s\n", buffer);

    // 5. 使用 snprintf 格式化输出到字符串，指定最大长度
    char small_buffer[10];
    snprintf(small_buffer, sizeof(small_buffer), "5. 答案: %d", num);
    printf("%s\n", small_buffer);

    // 6. 使用 scanf 格式化输入从标准输入
    int input_num;
    printf("6. 请输入一个数字: ");
    scanf("%d", &input_num);
    printf("你输入的是: %d\n", input_num);

    // 7. 使用 sscanf 格式化输入从字符串
    const char *str = "123 456";
    int a, b;
    sscanf(str, "%d %d", &a, &b);
    printf("7. 从字符串读取: %d, %d\n", a, b);

    // 8. 使用 fgets 从标准输入读取一行
    char input_str[100];
    printf("8. 请输入一行文本: ");
    fgets(input_str, sizeof(input_str), stdin);
    printf("你输入的是: %s", input_str);

    // 9. 使用 fputs 将字符串写入标准输出
    fputs("9. fputs: 这是一个测试字符串\n", stdout);

    // 10. 使用 getchar 从标准输入读取一个字符
    printf("10. 请输入一个字符: ");
    int ch = getchar();
    printf("你输入的字符是: %c\n", ch);
}

void demo_error_handling() {
    // 1. feof 示例
    {
        FILE *file = fopen("example.txt", "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }

        while (!feof(file)) {
            char buffer[100];
            if (fgets(buffer, sizeof(buffer), file) != NULL) {
                printf("%s", buffer);
            }
        }

        fclose(file);
    }

    // 2. ferror 示例
    {
        FILE *file = fopen("example.txt", "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }

        char buffer[100];
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            if (ferror(file)) {
                perror("Error reading file");
            }
        }

        fclose(file);
    }

    // 3. perror 示例
    {
        FILE *file = fopen("nonexistent.txt", "r");
        if (file == NULL) {
            perror("Error opening file");
        }
    }

    // 4. clearerr 示例
    {
        FILE *file = fopen("example.txt", "r");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }

        // 读取文件内容
        char buffer[100];
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            if (ferror(file)) {
                perror("Error reading file");
            }
        }

        // 清除文件流的错误和 EOF 标志
        clearerr(file);

        fclose(file);
    }

    // 5. strerror 示例
    {
        FILE *file = fopen("nonexistent.txt", "r");
        if (file == NULL) {
            printf("Error opening file: %s\n", strerror(errno));
        }
    }
}

int demo_file_io(){
    // 创建一个新文件，文件所有者具有读写权限，文件所属组和其他用户具有读权限
    int fd = creat("example.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("creat");
        return 1;
    }
    printf("File created with fd: %d\n", fd);

    // 写入数据到文件
    const char *text = "Hello, World!";
    ssize_t bytes_written = write(fd, text, strlen(text));
    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }
    printf("Written %zd bytes to file\n", bytes_written);

    // 使用 lseek 设置文件偏移量到文件开头
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    // 读取文件数据
    char buffer[100];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    buffer[bytes_read] = '\0'; // 确保缓冲区以空字符结尾
    printf("Read %zd bytes from file: %s\n", bytes_read, buffer);

    // 复制文件描述符
    int fd_dup = dup(fd);
    if (fd_dup == -1) {
        perror("dup");
        close(fd);
        return 1;
    }
    printf("File descriptor duplicated with dup, fd_dup: %d\n", fd_dup);

    // 复制文件描述符到指定的文件描述符
    int fd_dup2 = dup2(fd, 10);
    if (fd_dup2 == -1) {
        perror("dup2");
        close(fd);
        close(fd_dup);
        return 1;
    }
    printf("File descriptor duplicated with dup2, fd_dup2: %d\n", fd_dup2);

    // 获取文件状态标志
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        close(fd);
        close(fd_dup);
        close(fd_dup2);
        return 1;
    }
    printf("File status flags: %d\n", flags);

    // 设置文件状态标志为非阻塞
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        close(fd);
        close(fd_dup);
        close(fd_dup2);
        return 1;
    }
    printf("File status flags set to non-blocking mode\n");

    // 关闭文件描述符
    close(fd);
    close(fd_dup);
    close(fd_dup2);

    return 0;
}

void demo_file_system(){
    char cwd[1024];
    struct stat statbuf;
    struct dirent *entry;
    DIR *dir;

    // 获取并输出当前工作目录
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("当前工作目录: %s\n", cwd);
    } else {
        perror("获取当前工作目录失败");
    }

    // 创建目录
    if (mkdir("testdir", 0755) == 0) {
        printf("目录 'testdir' 创建成功。\n");
    } else {
        perror("创建目录失败");
    }

    // 更改当前工作目录
    if (chdir("testdir") == 0) {
        printf("切换到目录 'testdir'。\n");
    } else {
        perror("切换目录失败");
    }

    // 创建文件
    int fd = open("testfile.txt", O_CREAT | O_WRONLY, 0644);
    if (fd != -1) {
        printf("文件 'testfile.txt' 创建成功。\n");
        close(fd);
    } else {
        perror("创建文件失败");
    }

    // 获取文件信息
    if (stat("testfile.txt", &statbuf) == 0) {
        printf("文件 'testfile.txt' 信息:\n");
        printf("大小: %ld 字节\n", statbuf.st_size);
        printf("权限: %o\n", statbuf.st_mode & 0777);
        printf("最后修改时间: %ld\n", statbuf.st_mtime);
    } else {
        perror("获取文件信息失败");
    }

    // 修改文件权限
    if (chmod("testfile.txt", 0600) == 0) {
        printf("文件 'testfile.txt' 的权限已更改为 0600。\n");
    } else {
        perror("修改文件权限失败");
    }

    // 创建符号链接
    if (symlink("testfile.txt", "testfile_symlink.txt") == 0) {
        printf("符号链接 'testfile_symlink.txt' 创建成功。\n");
    } else {
        perror("创建符号链接失败");
    }

    // 创建硬链接
    if (link("testfile.txt", "testfile_hardlink.txt") == 0) {
        printf("硬链接 'testfile_hardlink.txt' 创建成功。\n");
    } else {
        perror("创建硬链接失败");
    }

    // 打开目录
    dir = opendir(".");
    if (dir != NULL) {
        printf("目录打开成功。\n");

        // 读取目录内容
        while ((entry = readdir(dir)) != NULL) {
            printf("名称: %s, 类型: %d\n", entry->d_name, entry->d_type);
        }

        // 关闭目录
        closedir(dir);
    } else {
        perror("打开目录失败");
    }

    // 删除符号链接
    if (unlink("testfile_symlink.txt") == 0) {
        printf("符号链接 'testfile_symlink.txt' 删除成功。\n");
    } else {
        perror("删除符号链接失败");
    }

    // 删除硬链接
    if (unlink("testfile_hardlink.txt") == 0) {
        printf("硬链接 'testfile_hardlink.txt' 删除成功。\n");
    } else {
        perror("删除硬链接失败");
    }

    // 删除文件
    if (remove("testfile.txt") == 0) {
        printf("文件 'testfile.txt' 删除成功。\n");
    } else {
        perror("删除文件失败");
    }

    // 更改回上一级目录
    if (chdir("..") == 0) {
        printf("切换回上一级目录。\n");
    } else {
        perror("切换目录失败");
    }

    // 删除目录
    if (rmdir("testdir") == 0) {
        printf("目录 'testdir' 删除成功。\n");
    } else {
        perror("删除目录失败");
    }
}

void server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *response = "hello world";

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 接受连接
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 读取客户端发送的数据
    read(new_socket, buffer, 1024);
    printf("服务器接收到的数据: %s\n", buffer);

    // 发送响应数据
    send(new_socket, response, strlen(response), 0);
    printf("服务器发送的数据: %s\n", response);

    // 关闭套接字
    close(new_socket);
    close(server_fd);
}

void client() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    const char *message = "hello";

    // 创建套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 将服务器地址转换为二进制形式
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    // 连接服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    // 发送数据到服务器
    send(sock, message, strlen(message), 0);
    printf("客户端发送的数据: %s\n", message);

    // 读取服务器响应的数据
    read(sock, buffer, 1024);
    printf("客户端接收到的数据: %s\n", buffer);

    // 关闭套接字
    close(sock);
}