#include<stdio.h>
#include<string.h>

#include "sms_func.h"

// 截取字符串尾部空格
void Rtrim(char* string)
{
    size_t len = 0;
    if(NULL == string)
        return;
    
    len = strlen(string);
    while(len>0 && string[len-1] == ' ')
    {
        string[--len] = '\0';
    }
    return;
}

// 截取字符串首部空格
void Ltrim(char* string)
{
    if(NULL == string)
        return;
    
    char* p_tmp = string;
    // 找到第一个不是空格的
    while(*p_tmp != '\0')
    {
        if(*p_tmp == ' ')
            p_tmp++;
        else
            break;
    }

    if(*p_tmp == '\0') // 全是空格
    {
        *string = '\0';
        return;
    }
    char *p_tmp2 = string; 
	while(*p_tmp != '\0')
	{
		*p_tmp2 = *p_tmp;
		p_tmp++;
		p_tmp2++;
	}
	*p_tmp2 = '\0';
    return;
    
}