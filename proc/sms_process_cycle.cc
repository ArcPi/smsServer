//和开启子进程相关

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>   //信号相关头文件 
#include <errno.h>    //errno
#include <unistd.h>

#include "sms_func.h"
#include "sms_macro.h"
#include "sms_c_conf.h"

//函数声明
static void sms_start_worker_processes(int threadnums);
static int sms_spawn_process(int threadnums,const char *pprocname);
static void sms_worker_process_cycle(int inum,const char *pprocname);
static void sms_worker_process_init(int inum);

//变量声明
static u_char  master_process[] = "master process";

//描述：创建worker子进程
void sms_master_process_cycle()
{    
    sigset_t set;        //信号集

    sigemptyset(&set);   //清空信号集

    sigaddset(&set, SIGCHLD);     //子进程状态改变
    sigaddset(&set, SIGALRM);     //定时器超时
    sigaddset(&set, SIGIO);       //异步I/O
    sigaddset(&set, SIGINT);      //终端中断符
    sigaddset(&set, SIGHUP);      //连接断开
    sigaddset(&set, SIGUSR1);     //用户定义信号
    sigaddset(&set, SIGUSR2);     //用户定义信号
    sigaddset(&set, SIGWINCH);    //终端窗口大小改变
    sigaddset(&set, SIGTERM);     //终止
    sigaddset(&set, SIGQUIT);     //终端退出符

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) 
    {        
        sms_log_error_core(SMS_LOG_ALERT,errno,"sms_master_process_cycle()中sigprocmask()失败!");
    }
    size_t size;
    int    i;
    size = sizeof(master_process);  
    size += g_argvneedmem;             
    if(size < 1000) 
    {
        char title[1000] = {0};
        strcpy(title,(const char *)master_process); 
        strcat(title," ");  
        for (i = 0; i < g_os_argc; i++)   
        {
            strcat(title,g_os_argv[i]);
        }//end for
        sms_setproctitle(title); //设置标题
    }
        
    CConfig *p_config = CConfig::GetInstance(); //单例类
    int workprocess = p_config->GetIntDefault("WorkerProcesses",1); //从配置文件中得到要创建的worker进程数量
    sms_start_worker_processes(workprocess);  //这里要创建worker子进程

    sigemptyset(&set); //信号屏蔽字为空，表示不屏蔽任何信号
    
    for ( ;; ) 
    {

        sms_log_error_core(0,0,"haha--这是父进程，pid为%P",sms_pid);
    }// end for(;;)
    return;
}

static void sms_start_worker_processes(int threadnums)
{
    int i;
    for (i = 0; i < threadnums; i++)  //master进程在走这个循环，来创建若干个子进程
    {
        sms_spawn_process(i,"worker process");
    } //end for
    return;
}

static int sms_spawn_process(int inum,const char *pprocname)
{
    pid_t  pid;

    pid = fork(); //fork()系统调用产生子进程
    switch (pid)  //pid判断父子进程，分支处理
    {  
    case -1: //产生子进程失败
        sms_log_error_core(SMS_LOG_ALERT,errno,"sms_spawn_process()fork()产生子进程num=%d,procname=\"%s\"失败!",inum,pprocname);
        return -1;

    case 0:  //子进程分支
        sms_parent = sms_pid;              
        sms_pid = getpid();               
        sms_worker_process_cycle(inum,pprocname);
        break;

    default:
        break;
    }//end switch

    return pid;
}

static void sms_worker_process_cycle(int inum,const char *pprocname) 
{
    sms_worker_process_init(inum);
    sms_setproctitle(pprocname); 

    for(;;)
    {

        sms_log_error_core(0,0,"good--这是子进程，编号为%d,pid为%P！",inum,sms_pid);

    } //end for(;;)
    return;
}

static void sms_worker_process_init(int inum)
{
    sigset_t  set;      //信号集

    sigemptyset(&set);  //清空信号集
    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) 
    {
        sms_log_error_core(SMS_LOG_ALERT,errno,"sms_worker_process_init()中sigprocmask()失败!");
    }
    
    return;
}
