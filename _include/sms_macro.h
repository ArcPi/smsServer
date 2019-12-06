
#ifndef __SMS_MACRO_H__
#define __SMS_MACRO_H__

//各种#define宏定义相关的定义放这里

#define SMS_MAX_ERROR_STR   2048   //显示的错误信息最大数组长度

//简单功能函数--------------------
#define sms_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n)) 
#define sms_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))             

//数字相关--------------------
#define SMS_MAX_UINT32_VALUE   (uint32_t) 0xffffffff              //最大的32位无符号数：十进制是‭4294967295‬
#define SMS_INT64_LEN          (sizeof("-9223372036854775808") - 1)     

//日志相关--------------------
#define SMS_LOG_STDERR            0    //控制台错误【stderr】：最高级别日志，日志的内容不再写入log参数指定的文件，而是会直接将日志输出到标准错误设备比如控制台屏幕
#define SMS_LOG_EMERG             1    //紧急 【emerg】
#define SMS_LOG_ALERT             2    //警戒 【alert】
#define SMS_LOG_CRIT              3    //严重 【crit】
#define SMS_LOG_ERR               4    //错误 【error】：属于常用级别
#define SMS_LOG_WARN              5    //警告 【warn】：属于常用级别
#define SMS_LOG_NOTICE            6    //注意 【notice】
#define SMS_LOG_INFO              7    //信息 【info】
#define SMS_LOG_DEBUG             8    //调试 【debug】：最低级别

#define SMS_ERROR_LOG_PATH       "logs/error1.log"   //定义日志存放的路径和文件名 


#endif
