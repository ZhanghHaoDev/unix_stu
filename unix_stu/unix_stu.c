#include "unix_stu.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/mman.h>

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

int demo_processes1(){
    // 1. exit 函数演示（由于 exit 会立即终止程序，这里不实际调用）

    // 2. 获取进程 ID 和用户 ID
    pid_t pid = getpid();
    pid_t ppid = getppid();
    uid_t uid = getuid();
    uid_t euid = geteuid();
    gid_t gid = getgid();
    gid_t egid = getegid();

    printf("当前进程 ID: %d\n", pid);
    printf("父进程 ID: %d\n", ppid);
    printf("用户 ID: %d\n", uid);
    printf("有效用户 ID: %d\n", euid);
    printf("组 ID: %d\n", gid);
    printf("有效组 ID: %d\n", egid);

    // 3. fork 函数演示
    pid_t fork_pid = fork();
    if (fork_pid < 0) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    } else if (fork_pid == 0) {
        // 子进程代码
        printf("这是子进程，进程 ID: %d，父进程 ID: %d\n", getpid(), getppid());

        // 5. exec 函数演示，使用 execlp 执行 ls 命令
        execlp("ls", "ls", "-l", NULL);
        // 如果 exec 调用成功，下面的代码将不会执行
        perror("execlp 失败");
        exit(EXIT_FAILURE);
    } else {
        // 父进程代码
        printf("这是父进程，子进程 ID: %d\n", fork_pid);

        // 4. wait 函数演示，等待子进程结束
        int status;
        pid_t wpid = wait(&status);
        if (wpid == -1) {
            perror("wait 出错");
        } else {
            printf("子进程 %d 结束，退出状态: %d\n", wpid, WEXITSTATUS(status));
        }
    }

    // 7. 获取用户信息
    struct passwd *pw = getpwuid(uid);
    if (pw != NULL) {
        printf("用户名: %s\n", pw->pw_name);
        printf("用户主目录: %s\n", pw->pw_dir);
    } else {
        perror("无法获取用户信息");
    }

    char *login_name = getlogin();
    if (login_name != NULL) {
        printf("登录名: %s\n", login_name);
    } else {
        perror("无法获取登录名");
    }

    // 8. 获取进程时间
    struct tms t;
    clock_t clk = times(&t);
    if (clk == (clock_t)-1) {
        perror("times 调用失败");
    } else {
        printf("用户 CPU 时间: %ld\n", t.tms_utime);
        printf("系统 CPU 时间: %ld\n", t.tms_stime);
    }

    // 6. 进程会计（由于需要管理员权限，且可能影响系统，这里不演示）

    return 0;
}

int demo_processes2(){
    pid_t pid, ppid, pgid, sid;

    // 获取当前进程的进程 ID 和父进程 ID
    pid = getpid();
    ppid = getppid();
    printf("当前进程 ID: %d\n", pid);
    printf("父进程 ID: %d\n", ppid);

    // 获取当前进程的进程组 ID
    pgid = getpgrp();
    printf("当前进程组 ID: %d\n", pgid);

    // 获取当前进程的会话 ID
    sid = getsid(pid);
    printf("当前会话 ID: %d\n", sid);

    // 创建子进程
    pid_t fork_pid = fork();
    if (fork_pid < 0) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    } else if (fork_pid == 0) {
        // 子进程代码
        printf("\n--- 子进程 ---\n");
        pid_t child_pid = getpid();
        pid_t child_ppid = getppid();
        pid_t child_pgid = getpgrp();
        pid_t child_sid = getsid(child_pid);

        printf("子进程 ID: %d\n", child_pid);
        printf("子进程的父进程 ID: %d\n", child_ppid);
        printf("子进程的进程组 ID: %d\n", child_pgid);
        printf("子进程的会话 ID: %d\n", child_sid);

        // 设置子进程的进程组 ID
        if (setpgid(0, 0) == -1) {
            perror("setpgid 失败");
            exit(EXIT_FAILURE);
        }
        printf("子进程设置了新的进程组 ID\n");
        child_pgid = getpgrp();
        printf("子进程的新进程组 ID: %d\n", child_pgid);

        // 创建新的会话
        pid_t new_sid = setsid();
        if (new_sid == -1) {
            perror("setsid 失败");
            exit(EXIT_FAILURE);
        }
        printf("子进程创建了新的会话，新的会话 ID: %d\n", new_sid);

        // 再次获取子进程的会话 ID 和进程组 ID
        child_pgid = getpgrp();
        child_sid = getsid(0);
        printf("子进程的新进程组 ID: %d\n", child_pgid);
        printf("子进程的新会话 ID: %d\n", child_sid);

        printf("--- 子进程结束 ---\n");
        exit(EXIT_SUCCESS);
    } else {
        // 父进程代码
        // 等待子进程结束
        wait(NULL);
        printf("\n子进程已结束。\n");
    }

    return 0;
}

int demo_processes3() {
    pid_t pid;
    int pipefd[2];
    char buffer[128];
    const char *shm_name = "/test_shm";
    const char *sem_name = "/test_sem";

    // 1. 无名管道
    if (pipe(pipefd) == -1) {
        perror("pipe 失败");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程：读取管道
        close(pipefd[1]); // 关闭写端
        read(pipefd[0], buffer, sizeof(buffer));
        printf("子进程从管道读取到数据：%s\n", buffer);
        close(pipefd[0]); // 关闭读端
        exit(EXIT_SUCCESS);
    } else {
        // 父进程：写入管道
        close(pipefd[0]); // 关闭读端
        char *msg = "Hello from parent via pipe!";
        write(pipefd[1], msg, strlen(msg) + 1);
        close(pipefd[1]); // 关闭写端
        wait(NULL);
    }

    // 2. 命名管道（FIFO）
    const char *fifo_name = "/tmp/myfifo";

    // 创建命名管道
    if (mkfifo(fifo_name, 0666) == -1) {
        perror("mkfifo 失败");
        // 如果命名管道已存在，继续
    }

    pid = fork();
    if (pid == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程：读取命名管道
        int fd = open(fifo_name, O_RDONLY);
        read(fd, buffer, sizeof(buffer));
        printf("子进程从命名管道读取到数据：%s\n", buffer);
        close(fd);
        exit(EXIT_SUCCESS);
    } else {
        // 父进程：写入命名管道
        int fd = open(fifo_name, O_WRONLY);
        char *msg = "Hello from parent via FIFO!";
        write(fd, msg, strlen(msg) + 1);
        close(fd);
        wait(NULL);
        // 删除命名管道
        unlink(fifo_name);
    }

    // 3. POSIX 共享内存
    int shm_fd;
    char *shm_addr;

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open 失败");
        exit(EXIT_FAILURE);
    }

    ftruncate(shm_fd, 128);

    pid = fork();
    if (pid == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程：读取共享内存
        shm_addr = mmap(NULL, 128, PROT_READ, MAP_SHARED, shm_fd, 0);
        printf("子进程从共享内存读取到数据：%s\n", shm_addr);
        munmap(shm_addr, 128);
        close(shm_fd);
        exit(EXIT_SUCCESS);
    } else {
        // 父进程：写入共享内存
        shm_addr = mmap(NULL, 128, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        char *msg = "Hello via POSIX shared memory!";
        memcpy(shm_addr, msg, strlen(msg) + 1);
        munmap(shm_addr, 128);
        close(shm_fd);
        wait(NULL);
        // 删除共享内存
        shm_unlink(shm_name);
    }

    // 4. POSIX 信号量
    sem_t *sem;

    sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open 失败");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程：等待信号量
        sem_wait(sem);
        printf("子进程进入临界区\n");
        sleep(2); // 模拟操作
        printf("子进程离开临界区\n");
        sem_post(sem);
        sem_close(sem);
        exit(EXIT_SUCCESS);
    } else {
        // 父进程：等待信号量
        sem_wait(sem);
        printf("父进程进入临界区\n");
        sleep(2); // 模拟操作
        printf("父进程离开临界区\n");
        sem_post(sem);
        sem_close(sem);
        wait(NULL);
        // 删除信号量
        sem_unlink(sem_name);
    }

    return 0;
}