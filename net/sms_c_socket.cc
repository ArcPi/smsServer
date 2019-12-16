#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "sms_c_conf.h"
#include "sms_macro.h"
#include "sms_global.h"
#include "sms_func.h"
#include "sms_c_socket.h"

CSocket::CSocket()
{
    m_ListenPortCount = 1;
    return;
}

CSocket::~CSocket()
{
    std::vector<lpsms_listening_t>::iterator pos;
    for(pos = m_ListenSocketList.begin(); pos!=m_ListenSocketList.end();pos++)
    {
        delete(*pos);
    }
    m_ListenSocketList.clear();
    return;
}

//初始化函数【fork子进程之前干这个事情】
bool CSocket::Initialize()
{
    bool reco = sms_open_listening_sockets();
    return reco;
}

//监听端口
//在创建worker进程之前调用这个函数
bool CSocket::sms_open_listening_sockets()
{
    CConfig *p_config = CConfig::GetInstance();
    m_ListenPortCount = p_config->GetIntDefault("ListenPortCount", m_ListenPortCount);

    int                 isock;          //socket
    struct sockaddr_in  serv_addr;      //服务器的地址结构体
    int                 iport;          //端口
    char                strinfo[100];        //临时字符串

    //初始化相关
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));  //初始化
    serv_addr.sin_family = AF_INET;                     //选择IPV4协议族
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);      //监听本地所有的IP地址；INADDR_ANY表示一个服务器上所有的网卡多个IP地址都进行绑定端口号，进行监听

    for (int i = 0; i < m_ListenPortCount; i++)
    {
        isock = socket(AF_INET, SOCK_STREAM, 0);        //系统函数，成功返回非负描述符，出错返回-1
        if (isock == -1)
        {
            sms_log_stderr(errno,"CSoket::Initialize()中socket失败, i=%d", i);
            return false;
        }

        int reuseaddr = 1;  //1:打开对应的设置项
        if (setsockopt(isock, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(reuseaddr)) == -1)
        {
            sms_log_stderr(errno, "CSocket::Initialize()中setsockopt()失败，i=%d.", i);
            close(isock);
            return false;
        }

        //设置该socket为非阻塞
        if(setnonblocking(isock) == -1)
        {
            sms_log_stderr(errno, "CSocket::Initialize()中setnonblocking()失败，i=%d.", i);
            close(isock);
            return false;
        }

        //设置本服务器要监听的地址和端口
        strinfo[0] = 0;
        sprintf(strinfo, "ListenPort%d", i);
        iport = p_config->GetIntDefault(strinfo, 10000);
        serv_addr.sin_port = htons((in_port_t)iport);

        //绑定服务器地址结构体
        if(bind(isock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        {
            sms_log_stderr(errno, "CSocket::Initialize()中bind()失败，i=%d.", i);
            close(isock);
            return false;
        }

        //开始监听
        if(listen(isock, SMS_LISTEN_BACKLOG) == -1)
        {
            sms_log_stderr(errno, "CSocket::Initialize()中listen失败,i=%d.", i);
            close(isock);
            return false;
        }

        //可以，放到列表里边来
        lpsms_listening_t p_listensocketitem = new sms_listening_t;
        memset(p_listensocketitem, 0, sizeof(sms_listening_t));
        p_listensocketitem->port = iport;
        p_listensocketitem->fd = isock;
        sms_log_error_core(SMS_LOG_INFO, 0, "监听%d端口成功", iport);
        m_ListenSocketList.push_back(p_listensocketitem);
    } //end for (int i = 0; i < m_ListenPortCount; i++)
    return true;
}

//设置socket链接为非阻塞模式
bool CSocket::setnonblocking(int sockfd)
{
    int nb = 1; //1:非阻塞， 0:阻塞
    if(ioctl(sockfd, FIONBIO, &nb) == -1)
    {
        return false;
    }
    return true;
}

void CSocket::sms_close_listening_sockets()
{
    for(int i = 0; i < m_ListenPortCount; i++)
    {
        close(m_ListenSocketList[i]->fd);
        sms_log_error_core(SMS_LOG_INFO, 0, "关闭监听端口%d", m_ListenSocketList[i]->port);
    }

    return;
}
