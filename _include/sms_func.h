#ifndef __SMS_FUNC_H__
#define __SMS_FUNC_H__

// 裁剪左边的空格
void Ltrim(char* string);

// 裁剪右边的空格
void Rtrim(char* string);

// 设置可执行程序标题相关函数
void sms_init_setproctitle();
void sms_setproctitle(const char* title);

#endif