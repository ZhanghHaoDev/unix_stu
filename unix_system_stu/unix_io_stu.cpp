#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "unix_io_stu.h"

unix_io_stu::unix_io_stu() = default;

unix_io_stu::~unix_io_stu() = default;

void unix_io_stu::demo_standard_io() {
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

void unix_io_stu::demo_error_handling() {
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

int unix_io_stu::demo_file_io(){
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