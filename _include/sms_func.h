//函数声明放在这个头文件里-------------------------------------------

#ifndef __SMS_FUNC_H__
#define __SMS_FUNC_H__

#include <stdlib.h>

//字符串相关函数
void   Rtrim(char *string);
void   Ltrim(char *string);

//设置可执行程序标题相关函数
void   sms_init_setproctitle();
void   sms_setproctitle(const char *title);

//和日志，打印输出有关
void   sms_log_init();
void   sms_log_stderr(int err, const char *fmt, ...);
void   sms_log_error_core(int level,  int err, const char *fmt, ...);
u_char *sms_log_errno(u_char *buf, u_char *last, int err);
u_char *sms_slprintf(u_char *buf, u_char *last, const char *fmt, ...);
u_char *sms_vslprintf(u_char *buf, u_char *last,const char *fmt,va_list args);

//和信号/主流程相关相关
int    sms_init_signals();
void   sms_master_process_cycle();
int    sms_daemon();

#endif  