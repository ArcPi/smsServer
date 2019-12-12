#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sms_func.h"
#include "sms_c_conf.h"
#include "sms_macro.h"

int sms_daemon()
{
    //(1) fork一个子进程出来
    switch (fork())
    {
    case -1:
        sms_log_error_core(SMS_LOG_EMERG, errno, "sms_daemon()中fork()失败");
        return -1;
    case 0:
        //子进程
        break;    
    default:
        //父进程
        return 1;
    }
    sms_parent = sms_pid;
    sms_pid = getpid();

    //（2）脱离终端
    if(setsid() == -1)
    {
        sms_log_error_core(SMS_LOG_EMERG, errno, "sms_daemon()在setsid()失败！");
        return -1;
    }

    //（3）设置文件权限为0
    umask(0);
    //(4)打开黑洞设备，以读写方式打开
    int fd = open("dev/null", O_RDWR);
    if(fd == -1)
    {
        sms_log_error_core(SMS_LOG_EMERG, errno, "sms_daemon()中打开黑洞设备失败");
        return -1;
    }
    if(dup2(fd, STDIN_FILENO) == -1) //关闭标准输入，将STDIN_FILENO指向fd
    {
        sms_log_error_core(SMS_LOG_EMERG, errno, "sms_daemon()中dup2(STDIN)失败");
        return -1;
    }
    if(dup2(fd, STDOUT_FILENO) == -1)
    {
        sms_log_error_core(SMS_LOG_EMERG, errno, "sms_daemon()中dup2(STDOUT)失败");
        return -1;
    }
    if(fd > STDERR_FILENO)
    {
        if(close(fd) == -1)
        {
            sms_log_error_core(SMS_LOG_EMERG, errno, "sms_daemon()中close(fd)失败");
            return -1;
        }
    }
    return 0;
}