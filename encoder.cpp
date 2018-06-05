#include "mbed.h"
#include "encoder.h"

encoder::encoder(PinName int_pin):InterruptIn(int_pin)
{
    sum = 0;
    state = 0;
}

void encoder::start()
{
    //bind callbacks
    rise(callback(this,&encoder::callback_rising));
    fall(callback(this,&encoder::callback_falling));
}

void encoder::reset()
{
    //unbind callbacks
    rise(NULL);
    fall(NULL);
}

int encoder::read()
{
    return sum;
}

void encoder::callback_rising()
{
    sum++;
}

void encoder::callback_falling()
{
    sum++;
}
