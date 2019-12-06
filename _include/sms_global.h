
#ifndef __SMS_GBLDEF_H__
#define __SMS_GBLDEF_H__


//类型定义----------------

//结构定义
typedef struct
{
	char ItemName[50];
	char ItemContent[500];
}CConfItem,*LPCConfItem;

//和运行日志相关 
typedef struct
{
	int    log_level;   //日志级别 或者日志类型，sms_macro.h里分0-8共9个级别
	int    fd;          //日志文件描述符

}sms_log_t;


//外部全局量声明
extern size_t      g_argvneedmem;
extern size_t      g_envneedmem; 
extern int         g_os_argc; 
extern char        **g_os_argv;
extern char        *gp_envmem; 

extern pid_t       sms_pid;
extern pid_t       sms_parent;
extern sms_log_t   sms_log;

#endif
