#ifndef __SMS_MACRO_H__
#define __SMS_MACRO_H__

#define SMS_MAX_ERROR_STR   2048    //显示的错误信息最大长度数组

// 简单功能函数
#define sms_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))
#define sms_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))             

#define SMS_MAX_UINT32_VALUE    (uint32_t) 0xffffffff
#define SMS_INT64_LEN          (sizeof("-9223372036854775808") - 1)     



// 日志等级
#define SMS_LOG_STDERR      0   // 控制台错误[stderr]
#define SMS_LOG_EMERG       1   // 紧急[emerg]
#define SMS_LOG_ALERT       2   // 警戒[alert]
#define SMS_LOG_CRIT        3   // 严重[crit]
#define SMS_LOG_ERR         4   // 错误[error] : 常用级别
#define SMS_LOG_WARN        5   // 警告[warn] : 常用级别
#define SMS_LOG_NOTICE      6   // 注意[notice]
#define SMS_LOG_INFO        7   // 信息[info]
#define SMS_LOG_DEBUG       8   // 调试[debug]

#define SMS_ERROR_LOG_PATH  "logs/error1.log"   //定义日志存放的路径和文件名



#endif