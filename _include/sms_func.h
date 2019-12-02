#ifndef __SMS_FUNC_H__
#define __SMS_FUNC_H__

#include<stdlib.h>

// 裁剪左边的空格
void Ltrim(char* string);

// 裁剪右边的空格
void Rtrim(char* string);

// 设置可执行程序标题相关函数
void sms_init_setproctitle();
void sms_setproctitle(const char* title);


// 日志格式化输出
u_char *sms_slprintf(u_char* buf, u_char* last, const char* fmt, ...);
u_char *sms_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);

// 日志初始化
void sms_log_init();
// 日志标准错误(往屏幕打印错误信息)
void sms_log_stderr(int err, const char* fmt, ...);
// 日志错误等级
u_char* sms_log_errno(u_char* buf, u_char* last, int err);
// 往日志文件写日志
void sms_log_error_core(int level, int err, const char* fmt, ...);

#endif