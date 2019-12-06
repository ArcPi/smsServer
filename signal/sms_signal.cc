//和信号有关的函数放这里
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>    //信号相关头文件 
#include <errno.h>     //errno

#include "sms_macro.h"
#include "sms_func.h" 

//一个信号有关的结构 sms_signal_t
typedef struct 
{
    int           signo;       
    const  char   *signame;   

    void  (*handler)(int signo, siginfo_t *siginfo, void *ucontext); //函数指针,   siginfo_t:系统定义的结构
} sms_signal_t;

static void sms_signal_handler(int signo, siginfo_t *siginfo, void *ucontext); //static表示该函数只在当前文件内可见

sms_signal_t  signals[] = {
    // signo      signame             handler
    { SIGHUP,    "SIGHUP",           sms_signal_handler },        //终端断开信号，对于守护进程常用于reload重载配置文件通知--标识1
    { SIGINT,    "SIGINT",           sms_signal_handler },        //标识2   
	{ SIGTERM,   "SIGTERM",          sms_signal_handler },        //标识15
    { SIGCHLD,   "SIGCHLD",          sms_signal_handler },        //子进程退出时，父进程会收到这个信号--标识17
    { SIGQUIT,   "SIGQUIT",          sms_signal_handler },        //标识3
    { SIGIO,     "SIGIO",            sms_signal_handler },        //指示一个异步I/O事件【通用异步I/O信号】
    { SIGSYS,    "SIGSYS",           NULL               },        //我们想忽略这个信号，SIGSYS表示收到了一个无效系统调用，如果我们不忽略，进程会被操作系统杀死，--标识31
    { 0,         NULL,               NULL               }         //信号对应的数字至少是1，所以可以用0作为一个特殊标记
};

int sms_init_signals()
{
    sms_signal_t      *sig;  
    struct sigaction   sa;   

    for (sig = signals; sig->signo != 0; sig++)  
    {        
        
        memset(&sa,0,sizeof(struct sigaction));

        if (sig->handler) 
        {
            sa.sa_sigaction = sig->handler;  
            sa.sa_flags = SA_SIGINFO;        
        }
        else
        {
            sa.sa_handler = SIG_IGN;
        } //end if

        sigemptyset(&sa.sa_mask);   
        
        if (sigaction(sig->signo, &sa, NULL) == -1) 
        {   
            sms_log_error_core(SMS_LOG_EMERG,errno,"sigaction(%s) failed",sig->signame); //显示到日志文件中去的 
            return -1; //有失败就直接返回
        }	
        else
        {            
            sms_log_stderr(0,"sigaction(%s) succed!",sig->signame); 
        }
    } //end for
    return 0; //成功    
}

//信号处理函数
static void sms_signal_handler(int signo, siginfo_t *siginfo, void *ucontext)
{
    printf("来信号了\n");
}
