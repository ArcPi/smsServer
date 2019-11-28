#ifndef __SMS_GLOBAL_H__
#define __SMS_GLOBAL_H__

// 比较通用的定义放在这里

// 读取配置的结构体定义
typedef struct
{
    char ItemName[50];
    char ItemContent[500];
}CConfigItem;

// 外部全局声明，和设置标题相关
extern char**   g_os_argv;
extern char*    gp_envmem;
extern int      g_environlen;

#endif