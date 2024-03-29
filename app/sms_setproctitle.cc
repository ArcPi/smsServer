#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "sms_global.h"
#include "sms_func.h"

// 设置标题前的初始化工作：分配内存，把环境变量拷贝到新内存来
void sms_init_setproctitle()
{

    gp_envmem = new char[g_envneedmem];
    memset(gp_envmem, 0, g_envneedmem);

    char* ptmp = gp_envmem;
    // 把原来的内容搬到新内存来
    for(size_t i = 0; environ[i];++i)
    {
        size_t size = strlen(environ[i]) + 1;
        strcpy(ptmp, environ[i]);
        environ[i] = ptmp;
        ptmp += size;
    }

    return;
}

// 设置可执行程序标题
void sms_setproctitle(const char* title)
{
    // 计算tile长度
    size_t ititlelen = strlen(title);

    size_t esy = g_argvneedmem + g_envneedmem;
    if(esy <= ititlelen)
    {
        // title过长，内存放不下
        return;
    }

    //设置后续命令行参数为空，只留g_os_argv[0]工作
    for(size_t i = 1; g_os_argv[i]; ++i)
    {
        g_os_argv[i] = NULL;
    }

    memset(g_os_argv[0], 0, esy);
    strcpy(g_os_argv[0], title);

    return;
}