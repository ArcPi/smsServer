#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<vector>

#include "sms_func.h"       // 通用函数
#include "sms_c_conf.h"     // 读取配置相关的类，放在该头文件中

// 构造函数
CConfig::CConfig()
{
}

bool CConfig::Load(const char* pconfName)
{
    // 创建文件描述符
    FILE *fp = fopen(pconfName, "r");
    if(NULL == fp)
    {
        return false;
    }

    // 临时存放每一行的配置文件
    char linebuf[501];

    while (!feof(fp))
    {
        // 处理读取一行失败的情况
        if(fgets(linebuf, 500, fp) == NULL)
            continue;
        // 处理开头便是'\0'的情况
        if(*linebuf=='\0')
            continue;
        // 处理注释行的情况，只要每行开头有这些字符，直接跳过
        if(*linebuf==';' || *linebuf==' ' || *linebuf=='#' || *linebuf=='\t')
            continue;
        
    lblprocstring:
        // 裁剪linebuf末尾的换行、回车、空格
        if(strlen(linebuf)>0)
        {
            if(linebuf[strlen(linebuf)-1] == 10 || linebuf[strlen(linebuf)-1] == 13 || linebuf[strlen(linebuf)-1] == 32)
                {
                    linebuf[strlen(linebuf)-1] = 0;
                    goto lblprocstring;
                }
        }
        // 经过裁剪，linebuf若无内容，重新读取新的一行
        if(*linebuf == 0)
            continue;
        // '['开头也重新读取一行
        if(*linebuf == '[')
            continue;
        // 类似"ListenPort = 5678"能走下来
        char* ptmp = strchr(linebuf, '=');
        if(ptmp != NULL)
        {
            std::shared_ptr<CConfigItem> sp_confitem= std::make_shared<CConfigItem>();
            strncpy(sp_confitem->ItemName, linebuf, (int)(ptmp-linebuf)); // "ListenPort "拷贝到p_confitem->ItemName
            strcpy(sp_confitem->ItemContent, ptmp+1); // " 5678"拷贝到p_confitem->ItemContent;
            
            Rtrim(sp_confitem->ItemName);
            Ltrim(sp_confitem->ItemName);
            Rtrim(sp_confitem->ItemContent);
            Ltrim(sp_confitem->ItemContent);

            m_ConfigItemList.push_back(std::move(sp_confitem));
        }
    } // end while

    fclose(fp);
    return true;
}

// 根据ItemName获取配置信息
const char* CConfig::GetString(const char* p_itemname)
{
    for(auto& lp_confitem : m_ConfigItemList)
    {
        if(strcasecmp(lp_confitem->ItemName, p_itemname) == 0)
            return lp_confitem->ItemContent;
    } // end for

    return NULL;
}

// 根据ItemName获取数字类型配置信息
int CConfig::GetIntDefault(const char* p_itemname, const int def)
{
    for(auto& lp_confitem : m_ConfigItemList)
    {
        if(strcasecmp(lp_confitem->ItemName, p_itemname) == 0)
            return atoi(lp_confitem->ItemContent);
    } // end for
    return def;
}