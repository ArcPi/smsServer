#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

#include "sms_c_conf.h"
#include "sms_func.h"

int main(int argc, char const *argv[])
{
    CConfig& config = CConfig::GetInstance(); // 单例类
    if(config.Load("sms.conf") == false)
    {
        printf("配置文件载入失败，推出\n");
        exit(1);
    }
    int port = config.GetIntDefault("ListenPort", 0);
    printf("port = %d\n", port);

    const char* test = config.GetString("dbinfo");
    printf("dbinfo = %s\n", test);
    return 0;
}
