#include "mbed.h"
#include "xy160d.h"

xy160d::xy160d(PinName pwmpin, PinName ctlpin1, PinName ctlpin2):PwmOut(pwmpin),ctl_1(ctlpin1),ctl_2(ctlpin2)
{
    ctl_1 = 0;
    ctl_2 = 0;
    mode_present = XY160D_BRAKE;
    period_ms(1);
    write(0.0f);
}

void xy160d::setmode(int mode_tmp)
{
    if(mode_present & mode_tmp  == 0)
    {
        ctl_1 = 0; ctl_2 = 0;
        wait_ms(20);
    }
    
    mode_present = mode_tmp;
    
    switch(mode_present)
    {
        case XY160D_FORWARD     : ctl_1 = 0; ctl_2 = 1; break;
        case XY160D_BACKWARD    : ctl_1 = 1; ctl_2 = 0; break;
        case XY160D_HANG        : ctl_1 = 1; ctl_2 = 1; break;
        case XY160D_BRAKE       : ctl_1 = 0; ctl_2 = 0; break;
        default:break;
    }
}

float xy160d::operator=(const float &value)
{
    write(value);
    return value;
}
