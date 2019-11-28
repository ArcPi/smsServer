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

int main(int argc, char const *argv[])
{
    g_os_argv = (char**) argv;
    sms_init_setproctitle();

    CConfig& config = CConfig::GetInstance(); // 单例类
    if(config.Load("sms.conf") == false)
    {
        printf("配置文件载入失败，推出\n");
        exit(1);
    }
    
    sms_setproctitle("sms: master process");

    for(;;)
    {
        sleep(2);
        printf("sleep one second\n");
    }

    return 0;
}
