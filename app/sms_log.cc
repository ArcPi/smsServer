#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>    //uintptr_t
#include <stdarg.h>    //va_start....
#include <unistd.h>    //STDERR_FILENO等
#include <sys/time.h>  //gettimeofday
#include <time.h>      //localtime_r
#include <fcntl.h>     //open
#include <errno.h>     //errno

#include "sms_global.h"
#include "sms_macro.h"
#include "sms_func.h"
#include "sms_c_conf.h"

static u_char err_levels[][20]  = 
{
    {"stderr"},    //0：控制台错误
    {"emerg"},     //1：紧急
    {"alert"},     //2：警戒
    {"crit"},      //3：严重
    {"error"},     //4：错误
    {"warn"},      //5：警告
    {"notice"},    //6：注意
    {"info"},      //7：信息
    {"debug"}      //8：调试
};
sms_log_t   sms_log;

void sms_log_stderr(int err, const char *fmt, ...)
{    
    va_list args;                        //创建一个va_list类型变量
    u_char  errstr[SMS_MAX_ERROR_STR+1]; 
    u_char  *p,*last;

    memset(errstr,0,sizeof(errstr));    
    last = errstr + SMS_MAX_ERROR_STR;        
    p = sms_cpymem(errstr, "sms: ", 5);
    
    va_start(args, fmt); 
    p = sms_vslprintf(p,last,fmt,args); 
    va_end(args);    

    if (err)
    {
        p = sms_log_errno(p, last, err);
    }
    
    //若位置不够，那换行也要硬插入到末尾，哪怕覆盖到其他内容    
    if (p >= (last - 1))
    {
        p = (last - 1) - 1; 
    }
    *p++ = '\n'; //增加个换行符    
    
    //往标准错误【一般是屏幕】输出信息    
    write(STDERR_FILENO,errstr,p - errstr); 

    return;
}

u_char *sms_log_errno(u_char *buf, u_char *last, int err)
{
    char *perrorinfo = strerror(err); 
    size_t len = strlen(perrorinfo);

    char leftstr[10] = {0}; 
    sprintf(leftstr," (%d: ",err);
    size_t leftlen = strlen(leftstr);

    char rightstr[] = ") "; 
    size_t rightlen = strlen(rightstr);
    
    size_t extralen = leftlen + rightlen; 
    if ((buf + len + extralen) < last)
    {
        buf = sms_cpymem(buf, leftstr, leftlen);
        buf = sms_cpymem(buf, perrorinfo, len);
        buf = sms_cpymem(buf, rightstr, rightlen);
    }
    return buf;
}

void sms_log_error_core(int level,  int err, const char *fmt, ...)
{
    u_char  *last;
    u_char  errstr[SMS_MAX_ERROR_STR+1];  

    memset(errstr,0,sizeof(errstr));  
    last = errstr + SMS_MAX_ERROR_STR;   
    
    struct timeval   tv;
    struct tm        tm;
    time_t           sec;   
    u_char           *p;    
    va_list          args;

    memset(&tv,0,sizeof(struct timeval));    
    memset(&tm,0,sizeof(struct tm));

    gettimeofday(&tv, NULL);     //获取当前时间，返回自1970-01-01 00:00:00到现在经历的秒数【第二个参数是时区，一般不关心】        

    sec = tv.tv_sec;             //秒
    localtime_r(&sec, &tm);      //把参数1的time_t转换为本地时间，保存到参数2中去，带_r的是线程安全的版本，尽量使用
    tm.tm_mon++;                 //月份要调整下正常
    tm.tm_year += 1900;          //年份要调整下才正常
    
    u_char strcurrtime[40]={0};  //先组合出一个当前时间字符串，格式形如：2019/01/08 19:57:11
    sms_slprintf(strcurrtime,  
                    (u_char *)-1,                       //若用一个u_char *接一个 (u_char *)-1,则 得到的结果是 0xffffffff....，这个值足够大
                    "%4d/%02d/%02d %02d:%02d:%02d",     //格式是 年/月/日 时:分:秒
                    tm.tm_year, tm.tm_mon,
                    tm.tm_mday, tm.tm_hour,
                    tm.tm_min, tm.tm_sec);
    p = sms_cpymem(errstr,strcurrtime,strlen((const char *)strcurrtime));  
    p = sms_slprintf(p, last, " [%s] ", err_levels[level]);                
    p = sms_slprintf(p, last, "%P: ",sms_pid);

    va_start(args, fmt);                     //使args指向起始的参数
    p = sms_vslprintf(p, last, fmt, args);   //把fmt和args参数弄进去，组合出来这个字符串
    va_end(args);                            //释放args 

    if (err)  //如果错误代码不是0，表示有错误发生
    {
        //错误代码和错误信息也要显示出来
        p = sms_log_errno(p, last, err);
    }
    //若位置不够，那换行也要硬插入到末尾，哪怕覆盖到其他内容
    if (p >= (last - 1))
    {
        p = (last - 1) - 1; //把尾部空格留出来，这里感觉nginx处理的似乎就不对 
    }
    *p++ = '\n'; //增加个换行符       

    ssize_t   n;
    while(1) 
    {        
        if (level > sms_log.log_level) 
        {
            break;
        }

        //写日志文件        
        n = write(sms_log.fd,errstr,p - errstr);  //文件写入成功后，如果中途
        if (n == -1) 
        {
            //写失败有问题
            if(errno == ENOSPC) //写失败，且原因是磁盘没空间了
            {
                //磁盘没空间了
                //没空间还写个毛线啊
                //先do nothing吧；
            }
            else
            {
                //这是有其他错误，那么我考虑把这个错误显示到标准错误设备吧；
                if(sms_log.fd != STDERR_FILENO) //当前是定位到文件的，则条件成立
                {
                    n = write(STDERR_FILENO,errstr,p - errstr);
                }
            }
        }
        break;
    } //end while    
    return;
}

void sms_log_init()
{
    u_char *plogname = NULL;

    //从配置文件中读取和日志相关的配置信息
    CConfig *p_config = CConfig::GetInstance();
    plogname = (u_char *)p_config->GetString("Log");
    if(plogname == NULL)
    {
        plogname = (u_char *) SMS_ERROR_LOG_PATH; //"logs/error.log" ,logs目录需要提前建立出来
    }
    sms_log.log_level = p_config->GetIntDefault("LogLevel",SMS_LOG_NOTICE);//缺省日志等级为6【注意】 ，如果读失败，就给缺省日志等级

    sms_log.fd = open((const char *)plogname,O_WRONLY|O_APPEND|O_CREAT,0644);  
    if (sms_log.fd == -1)  //如果有错误，则直接定位到 标准错误上去 
    {
        sms_log_stderr(errno,"[alert] could not open error log file: open() \"%s\" failed", plogname);
        sms_log.fd = STDERR_FILENO; //直接定位到标准错误去了        
    } 
    return;
}
