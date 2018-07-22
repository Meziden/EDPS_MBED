#include "mbed.h"
//该行在旧版本中需要使用，若提示Thread namespace错误请加入下一行
//#include "rtos.h"
#include "l298n.h"

l298n::l298n(PinName pin1,PinName pin2,PinName pin3,PinName pin4):out1(pin1),out2(pin2),out3(pin3),out4(pin4)
{
    stepcount = 0;
}

int l298n::rotate(int time_ms)
{   
    int steps = stepcount;
    out1=out2=out3=out4=0;
    
    while(stepcount != 0)
    {
        if(stepcount > 0)
        {
            switch(stepcount % 8)
            {
                case 7:
                out2=out3=out4=0;out1=1;break;
                case 6:
                out3=out4=0;out1=out2=1;break;
                case 5:
                out1=out2=out4=0;out2=1;break;
                case 4:
                out1=out4=0;out2=out3=1;break;
                case 3:
                out1=out2=out4=0;out3=1;break;
                case 2:
                out1=out2=0;out3=out4=1;break;
                case 1:
                out1=out2=out3=0;out4=1;break;
                case 0:
                out2=out3=0;out4=out1=1;break;
            }
            stepcount--;
        }
           
        else//处理负数输入为反向转动
        {
            switch(stepcount % 8)
            {
                case 0:
                out2=out3=out4=0;out1=1;break;
                case -1:
                out3=out4=0;out1=out2=1;break;
                case -2:
                out1=out2=out4=0;out2=1;break;
                case -3:
                out1=out4=0;out2=out3=1;break;
                case -4:
                out1=out2=out4=0;out3=1;break;
                case -5:
                out1=out2=0;out3=out4=1;break;
                case -6:
                out1=out2=out3=0;out4=1;break;
                case -7:
                out2=out3=0;out4=out1=1;break;
            }
            stepcount++;
        }
        Thread::wait(time_ms);
    }
    out1=out2=out3=out4=0;
     //返回已转步数
    return steps - stepcount;
}
