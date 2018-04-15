#include "mbed.h"

DigitalOut ctl1(p7);
DigitalOut ctl2(p8);
PwmOut motor1(p22);

int main()
{
    ctl1 = 0;
    ctl2 = 0;
    //set pwm frequency
    motor1.period_ms(1);
    //set dutycycle
    motor1.write(0.0);
    return 0;
}
