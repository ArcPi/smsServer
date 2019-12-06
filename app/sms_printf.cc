#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "sms_global.h"
#include "sms_func.h"
#include "sms_macro.h"

static u_char *sms_sprintf_num(u_char *buf, u_char *last, uint64_t ui64, u_char zero, uintptr_t hexadecimal, uintptr_t width)
{
    u_char *p, temp[SMS_INT64_LEN + 1]; 
    size_t len;
    uint32_t ui32;

    static u_char hex[] = "0123456789abcdef"; 
    static u_char HEX[] = "0123456789ABCDEF"; 

    p = temp + SMS_INT64_LEN; 

    if (hexadecimal == 0)
    {
        if (ui64 <= (uint64_t)SMS_MAX_UINT32_VALUE)
        {
            ui32 = (uint32_t)ui64; //能保存下
            do                     
            {
                *--p = (u_char)(ui32 % 10 + '0');
            } while (ui32 /= 10);                
        }
        else
        {
            do
            {
                *--p = (u_char)(ui64 % 10 + '0');
            } while (ui64 /= 10);
        }
    }
    else if (hexadecimal == 1) 
    {
        do
        {
            *--p = hex[(uint32_t)(ui64 & 0xf)];
        } while (ui64 >>= 4); 
                             
    }
    else 
    {
        do
        {
            *--p = HEX[(uint32_t)(ui64 & 0xf)];
        } while (ui64 >>= 4);
    }

    len = (temp + SMS_INT64_LEN) - p; // 得到这个数字的宽度，比如 “7654321”这个数字 ,len = 7

    while (len++ < width && buf < last) // 如果希望显示的宽度是10个宽度【%12f】，而实际想显示的是7654321，只有7个宽度，那么这里要填充5个0进去到末尾，凑够要求的宽度
    {
        *buf++ = zero;        
    }

    len = (temp + SMS_INT64_LEN) - p; 

    if ((buf + len) >= last)
    {
        len = last - buf; 
    }

    return sms_cpymem(buf, p, len); //把最新buf返回去；
}

u_char *sms_slprintf(u_char *buf, u_char *last, const char *fmt, ...)
{
    va_list args;
    u_char *p;

    va_start(args, fmt);
    p = sms_vslprintf(buf, last, fmt, args);
    va_end(args);
    return p;
}

u_char *sms_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args)
{
    u_char zero;

    uintptr_t width, sign, hex, frac_width, scale, n; // 临时用到的一些变量

    int64_t i64;   //保存%d对应的可变参
    uint64_t ui64; //保存%ud对应的可变参，临时作为%f可变参的整数部分也是可以的
    u_char *p;     //保存%s对应的可变参
    double f;      //保存%f对应的可变参
    uint64_t frac; //%f可变参数,根据%.2f等，取得小数部分的2位后的内容；

    while (*fmt && buf < last) //每次处理一个字符，处理的是  "invalid option: \"%s\",%d" 中的字符
    {
        if (*fmt == '%') //%开头的一般都是需要被可变参数 取代的
        {
            //  -----------------变量初始化工作开始-----------------
            //  ++fmt是先加后用，也就是fmt先往后走一个字节位置，然后再判断该位置的内容
            zero = (u_char)((*++fmt == '0') ? '0' : ' '); // 判断%后边接的是否是个'0',如果是zero = '0'，否则zero = ' '，一般比如你想显示10位，而实际数字7位，前头填充三个字符，就是这里的zero用于填充
                                                          //  ngx_log_stderr(0, "数字是%010d", 12);

            width = 0;      // 格式字符% 后边如果是个数字，这个数字最终会弄到width里边来 ,这东西目前只对数字格式有效，比如%d,%f这种
            sign = 1;       // 显示的是否是有符号数，这里给1，表示是有符号数，除非你 用%u，这个u表示无符号数
            hex = 0;        // 是否以16进制形式显示(比如显示一些地址)，0：不是，1：是，并以小写字母显示a-f，2：是，并以大写字母显示A-F
            frac_width = 0; // 小数点后位数字，一般需要和%.10f配合使用，这里10就是frac_width；
            i64 = 0;        // 一般用%d对应的可变参中的实际数字，会保存在这里
            ui64 = 0;       // 一般用%ud对应的可变参中的实际数字，会保存在这里

            // 这个while就是判断%后边是否是个数字，如果是个数字，就把这个数字取出来，比如%16，最终这个循环就能够把16取出来弄到width里边去
            // %16d 这里最终width = 16;
            while (*fmt >= '0' && *fmt <= '9') // 如果%后边接的字符是 '0' --'9'之间的内容   ，比如  %16这种；
            {
                // 第一次 ：width = 1;  第二次 width = 16，所以整个width = 16；
                width = width * 10 + (*fmt++ - '0');
            }

            for (;;) // 一些特殊的格式，我们做一些特殊的标记【给一些变量特殊值等等】
            {
                switch (*fmt) // 处理一些%之后的特殊字符
                {
                case 'u':     // %u，这个u表示无符号
                    sign = 0; // 标记这是个无符号数
                    fmt++;    // 往后走一个字符
                    continue; // 回到for继续判断

                case 'X':    // %X，X表示十六进制，并且十六进制中的A-F以大写字母显示，不要单独使用，一般是%Xd
                    hex = 2; // 标记以大写字母显示十六进制中的A-F
                    sign = 0;
                    fmt++;
                    continue;
                case 'x':    // %x，x表示十六进制，并且十六进制中的a-f以小写字母显示，不要单独使用，一般是%xd
                    hex = 1; // 标记以小写字母显示十六进制中的a-f
                    sign = 0;
                    fmt++;
                    continue;

                case '.':                              // 其后边必须跟个数字，必须与%f配合使用，形如 %.10f：表示转换浮点数时小数部分的位数，比如%.10f表示转换浮点数时，小数点后必须保证10位数字，不足10位则用0来填补；
                    fmt++;                             // 往后走一个字符，后边这个字符肯定是0-9之间，因为%.要求接个数字先
                    while (*fmt >= '0' && *fmt <= '9') // 如果是数字，一直循环，这个循环最终就能把诸如%.10f中的10提取出来
                    {
                        frac_width = frac_width * 10 + (*fmt++ - '0');
                    } // end while(*fmt >= '0' && *fmt <= '9')
                    break;

                default:
                    break;
                } // end switch (*fmt)
                break;
            } // end for ( ;; )

            switch (*fmt)
            {
            case '%': // 只有%%时才会遇到这个情形，本意是打印一个%，所以
                *buf++ = '%';
                fmt++;
                continue;

            case 'd':     // 显示整型数据，如果和u配合使用，也就是%ud,则是显示无符号整型数据
                if (sign) // 如果是有符号数
                {
                    i64 = (int64_t)va_arg(args, int); // va_arg():遍历可变参数，var_arg的第二个参数表示遍历的这个可变的参数的类型
                }
                else // 如何是和 %ud配合使用，则本条件就成立
                {
                    ui64 = (uint64_t)va_arg(args, u_int);
                }
                break; // 这break掉，直接跳道switch后边的代码去执行,这种凡是break的，都不做fmt++;  *********************【switch后仍旧需要进一步处理】

            case 's':                       // 一般用于显示字符串
                p = va_arg(args, u_char *); // va_arg():遍历可变参数，var_arg的第二个参数表示遍历的这个可变的参数的类型

                while (*p && buf < last) // 没遇到字符串结束标记，并且buf值够装得下这个参数
                {
                    *buf++ = *p++; // 那就装，比如  "%s"    ，   "abcdefg"，那abcdefg都被装进来
                }

                fmt++;
                continue; // 重新从while开始执行

            case 'P': // 转换一个pid_t类型
                i64 = (int64_t)va_arg(args, pid_t);
                sign = 1;
                break;

            case 'f':                     // 一般 用于显示double类型数据，如果要显示小数部分，则要形如 %.5f
                f = va_arg(args, double); // va_arg():遍历可变参数，var_arg的第二个参数表示遍历的这个可变的参数的类型
                if (f < 0)                //负数的处理
                {
                    *buf++ = '-'; // 单独搞个负号出来
                    f = -f;       // 那这里f应该是正数了!
                }
                // 走到这里保证f肯定 >= 0【不为负数】
                ui64 = (int64_t)f; // 正整数部分给到ui64里
                frac = 0;

                // 如果要求小数点后显示多少位小数
                if (frac_width) // 如果是%d.2f，那么frac_width就会是这里的2
                {
                    scale = 1; // 缩放从1开始
                    for (n = frac_width; n; n--)
                    {
                        scale *= 10; // 这可能溢出
                    }

                    // 把小数部分取出来 ，比如如果是格式    %.2f   ，对应的参数是12.537
                    // (uint64_t) ((12.537 - (double) 12) * 100 + 0.5);
                    //= (uint64_t) (0.537 * 100 + 0.5)  = (uint64_t) (53.7 + 0.5) = (uint64_t) (54.2) = 54
                    frac = (uint64_t)((f - (double)ui64) * scale + 0.5); // 取得保留的那些小数位数，【比如  %.2f   ，对应的参数是12.537，取得的就是小数点后的2位四舍五入，也就是54】
                                                                         // 如果是"%.6f", 21.378，那么这里frac = 378000

                    if (frac == scale) // 进位，比如    %.2f ，对应的参数是12.999，那么  = (uint64_t) (0.999 * 100 + 0.5)  = (uint64_t) (99.9 + 0.5) = (uint64_t) (100.4) = 100
                                       // 而此时scale == 100，两者正好相等
                    {
                        ui64++;   // 正整数部分进位
                        frac = 0; // 小数部分归0
                    }
                } // end if (frac_width)

                // 正整数部分，先显示出来
                buf = sms_sprintf_num(buf, last, ui64, zero, 0, width); // 把一个数字 比如“1234567”弄到buffer中显示

                if (frac_width) // 指定了显示多少位小数
                {
                    if (buf < last)
                    {
                        *buf++ = '.'; // 因为指定显示多少位小数，先把小数点增加进来
                    }
                    buf = sms_sprintf_num(buf, last, frac, '0', 0, frac_width); // frac这里是小数部分，显示出来，不够的，前边填充'0'字符
                }
                fmt++;
                continue; //重新从while开始执行

            default:
                *buf++ = *fmt++; // 往下移动一个字符
                continue;        // 注意这里不break，而是continue;而这个continue其实是continue到外层的while去了，也就是流程重新从while开头开始执行;
            }                    // end switch (*fmt)

            // 显示%d的，会走下来，其他走下来的格式日后逐步完善......
            // 统一把显示的数字都保存到 ui64 里去；
            if (sign) // 显示的是有符号数
            {
                if (i64 < 0) // 这可能是和%d格式对应的要显示的数字
                {
                    *buf++ = '-';          // 小于0，自然要把负号先显示出来
                    ui64 = (uint64_t)-i64; // 变成无符号数（正数）
                }
                else // 显示正数
                {
                    ui64 = (uint64_t)i64;
                }
            } // end if (sign)

            // 把一个数字 比如“1234567”弄到buffer中显示，如果是要求10位，则前边会填充3个空格比如“   1234567”
            // 注意第5个参数hex，是否以16进制显示，比如如果你是想以16进制显示一个数字则可以%Xd或者%xd，此时hex = 2或者1
            buf = sms_sprintf_num(buf, last, ui64, zero, hex, width);
            fmt++;
        }
        else // 当成正常字符，源【fmt】拷贝到目标【buf】里
        {
            // 用fmt当前指向的字符赋给buf当前指向的位置，然后buf往前走一个字符位置，fmt当前走一个字符位置
            *buf++ = *fmt++; // *和++优先级相同，结合性从右到左，所以先求的是buf++以及fmt++，但++是先用后加；
        }                    // end if (*fmt == '%')
    }                        // end while (*fmt && buf < last)
    return buf;
}