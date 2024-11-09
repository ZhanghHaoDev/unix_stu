#include "unix_system_stu.h"

#include <stdio.h>

#include <sys/syslog.h>
#include <syslog.h>

unix_system_stu::unix_system_stu() = default;
unix_system_stu::~unix_system_stu() = default;

void unix_system_stu::log_test(){
    // 设置日志掩码 过滤
    // INFO 以下的日志等级不输出
    setlogmask(LOG_INFO);

    syslog(LOG_INFO, "INFO的日志输出");
    syslog(LOG_DEBUG, "DEBUG 日志输出");

    closelog();
}