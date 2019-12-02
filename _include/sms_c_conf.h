#ifndef __SMS_C_CONFIG_H__
#define __SMS_C_CONFIG_H__

#include <vector>
#include <memory>

#include "sms_global.h"

// 单例类，读取配置文件
class CConfig
{
private:
    // 将默认构造函数设为私有
    // 禁用拷贝构造函数和拷贝复制函数
    CConfig();
    CConfig(const CConfig &) = delete;
    CConfig &operator=(const CConfig &) = delete;

public:
    static CConfig &GetInstance()
    {
        static CConfig instance;
        return instance;
    }

public:
    bool Load(const char *pconfName);                         // 装载配置文件
    const char *GetString(const char *p_itemname);            // 获取string
    int GetIntDefault(const char *p_itemname, const int def); // 获取int
public:
    std::vector<std::shared_ptr<CConfigItem>> m_ConfigItemList; // 存储配置信息
};

#endif