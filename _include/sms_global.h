#ifndef __SMS_GLOBAL_H__
#define __SMS_GLOBAL_H__

// 比较通用的定义放在这里

// 读取配置的结构体定义
typedef struct
{
    char ItemName[50];
    char ItemContent[500];
} CConfigItem;

// 日志等级
typedef struct
{
    int log_level; // 日志级别
    int fd;        // 日志文件描述符
} sms_log_t;

// 外部全局声明，和设置标题相关
extern char **g_os_argv;
extern char *gp_envmem;
extern int g_environlen;

extern pid_t        sms_pid;
extern sms_log_t    sms_log;

#endif