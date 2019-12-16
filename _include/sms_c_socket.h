#ifndef __SMS_C_SOCKET_H__
#define __SMS_C_SOCKET_H__

#include <vector>

#define SMS_LISTEN_BACKLOG 511

typedef struct sms_listening_s
{
    int     port;
    int     fd;
}sms_listening_t, *lpsms_listening_t;

class CSocket
{
public:
    CSocket();                                              //构造函数
    virtual ~CSocket();                                     //析构函数
public:
    virtual bool Initialize();                              //初始化函数
private:
    bool sms_open_listening_sockets();                      //监听必须的端口
    void sms_close_listening_sockets();                     //关闭监听套接字
    bool setnonblocking(int sockfd);                        //设置非阻碍套接字

private:
    int                             m_ListenPortCount;      //所监听的端口数量
    std::vector<lpsms_listening_t>  m_ListenSocketList;     //监听套接字队列
};

#endif