#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

#include "sms_c_conf.h"
#include "sms_func.h"
#include "sms_global.h"

char **g_os_argv;            
char *gp_envmem = NULL;      
int  g_environlen = 0;  

pid_t sms_pid;

void freeresource();

int main(int argc, char const *argv[])
{

    int exitcode = 0;

    sms_pid = getpid();
    g_os_argv = (char**) argv;

    CConfig& config = CConfig::GetInstance(); // 单例类
    if(config.Load("sms.conf") == false)
    {
        sms_log_stderr(0, "配置文件[%s]载入失败，退出", "sms.conf");
        exitcode = 2;
        goto lblexit;
    }
    
    //日志初始化
    sms_log_init();
    //设置标题初始化
    sms_init_setproctitle();


    for(;;)
    {
        sleep(2);
        printf("sleep one second\n");
    }

lblexit:
    freeresource();
    printf("程序退出，再见\n");
    return exitcode;

}

void freeresource()
{
    //(1)对于因为设置可执行程序标题导致的环境变量分配的内存，我们应该释放
    if(gp_envmem)
    {
        delete []gp_envmem;
        gp_envmem = NULL;
    }

    //(2)关闭日志文件
    if(sms_log.fd != STDERR_FILENO && sms_log.fd != -1)  
    {        
        close(sms_log.fd); //不用判断结果了
        sms_log.fd = -1; //标记下，防止被再次close吧        
    }
}
