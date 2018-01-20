#include "mbed.h"
#include "rtos.h"

class StepMotor
{
public:
    //在使用demux之后,StepMotor构造函数参数可以减至一个。
    //因mbed现不支持c++11,构造函数不可委托，默认构造函数也不可为delete。
    StepMotor(PinName pin1,PinName pin2,PinName pin3,PinName pin4);
    
    //主线程更改StepMotor::stepcount并使用信号使rotate函数执行。
    int stepcount;

    int rotate(int time);
    
private:
    DigitalOut out1,out2,out3,out4;
};