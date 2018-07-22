//L298N Step Motor Drive

#ifndef L298N_H
#define L298N_H

class l298n
{
public:
    l298n(PinName pin1,PinName pin2,PinName pin3,PinName pin4);
    
    //主线程更改l298n::stepcount并使用信号使rotate函数执行。
    int stepcount;

    int rotate(int time);
    
private:
    DigitalOut out1,out2,out3,out4;
};

#endif
