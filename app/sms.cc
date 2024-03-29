
//整个程序入口函数放这里

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sms_c_conf.h" //和配置文件处理相关的类,名字带c_表示和类有关
#include "sms_func.h"   //各种函数声明
#include "sms_c_socket.h"

//本文件用的函数声明
static void freeresource();

//和设置标题有关的全局量
size_t g_argvneedmem = 0; //保存下这些argv参数所需要的内存大小
size_t g_envneedmem = 0;  //环境变量所占内存大小
int g_os_argc;            //参数个数
char **g_os_argv;         //原始命令行参数数组,在main中会被赋值
char *gp_envmem = NULL;   //指向自己分配的env环境变量的内存，在sms_init_setproctitle()函数中会被分配内存
int g_daemonized = 0;     //守护进程标记，是否启用守护进程，0不启用，1启用

//socket相关
CSocket g_socket;

//和进程本身有关的全局量
pid_t sms_pid;    //当前进程的pid
pid_t sms_parent; //父进程的pid

int main(int argc, char *const *argv)
{
    int exitcode = 0; //退出代码，先给0表示正常退出
    int i;            //临时用

    //(1)无伤大雅也不需要释放的放最上边
    sms_pid = getpid();     //取得进程pid
    sms_parent = getppid(); //取得父进程的id
    //统计argv所占的内存
    g_argvneedmem = 0;
    for (i = 0; i < argc; i++) //argv =  ./nginx -a -b -c asdfas
    {
        g_argvneedmem += strlen(argv[i]) + 1; //+1是给\0留空间。
    }
    //统计环境变量所占的内存。注意判断方法是environ[i]是否为空作为环境变量结束标记
    for (i = 0; environ[i]; i++)
    {
        g_envneedmem += strlen(environ[i]) + 1; //+1是因为末尾有\0,是占实际内存位置的，要算进来
    }                                           //end for

    g_os_argc = argc;          //保存参数个数
    g_os_argv = (char **)argv; //保存参数指针

    //(2)初始化失败，就要直接退出的
    CConfig *p_config = CConfig::GetInstance(); //单例类
    if (p_config->Load("sms.conf") == false)    //把配置文件内容载入到内存
    {
        sms_log_stderr(0, "配置文件[%s]载入失败，退出!", "sms.conf");
        //exit(1);终止进程，在main中出现和return效果一样 ,exit(0)表示程序正常, exit(1)/exit(-1)表示程序异常退出，exit(2)表示表示系统找不到指定的文件
        exitcode = 2; //标记找不到文件
        goto lblexit;
    }

    //(3)一些初始化函数，准备放这里
    sms_log_init();              //日志初始化(创建/打开日志文件)
    if (sms_init_signals() != 0) //信号初始化
    {
        exitcode = 1;
        goto lblexit;
    }
    if(g_socket.Initialize() == false)
    {
        exitcode = 1;
        goto lblexit;
    }

    //(4)一些不好归类的其他类别的代码，准备放这里
    sms_init_setproctitle(); //把环境变量搬家

    //（5）创建守护进程
    if (p_config->GetIntDefault("Daemon", 0) == 1)
    {
        // 1:使用守护进程运行
        int cdaemonresult = sms_daemon();
        if (cdaemonresult == -1)
        {
            exitcode = 1;
            goto lblexit;
        }
        if (cdaemonresult == 1)
        {
            freeresource();
            exitcode = 0;
            return exitcode;
        }
        //成功创建守护进程
        g_daemonized = 1;
    }

    //(6)开始正式的主工作流程，主流程一致在下边这个函数里循环，暂时不会走下来，资源释放啥的日后再慢慢完善和考虑
    sms_master_process_cycle(); //不管父进程还是子进程，正常工作期间都在这个函数里循环；

    //--------------------------------------------------------------
    //for(;;)
    //{
    //    sleep(1); //休息1秒
    //    printf("休息1秒\n");
    //}

    //--------------------------------------
lblexit:
    //(5)该释放的资源要释放掉
    freeresource(); //一系列的main返回前的释放动作函数
    printf("程序退出，再见!\n");
    return exitcode;
}

void freeresource()
{
    if (gp_envmem)
    {
        delete[] gp_envmem;
        gp_envmem = NULL;
    }

    //(2)关闭日志文件
    if (sms_log.fd != STDERR_FILENO && sms_log.fd != -1)
    {
        close(sms_log.fd); //不用判断结果了
        sms_log.fd = -1;   //标记下，防止被再次close吧
    }
}
