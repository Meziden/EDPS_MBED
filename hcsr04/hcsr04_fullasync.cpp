#include "mbed.h"
#include "hcsr04.h"

hcsr04::hcsr04(PinName trig_pin, PinName recv_pin):trigger(trig_pin),receiver(recv_pin)
{
    distance  = 0;
    //setting up functions
    receiver.rise(callback(this,&hcsr04::callback_rising));
    receiver.fall(callback(this,&hcsr04::callback_falling));
    receiver.enable_irq();
    timeout.attach(callback(this,&hcsr04::callback_timeout),HCSR04_TIMEOUT);
}

float hcsr04::getdistance()
{   
    return distance;
}

void hcsr04::callback_timeout()
{
    //abort measurement
    timer.reset();
    receiver.rise(NULL);
    receiver.fall(NULL);
    
    distance = -1;
    
    //aynchronously - awaiting response
    receiver.rise(callback(this,&hcsr04::callback_rising));
    receiver.fall(callback(this,&hcsr04::callback_falling));
    receiver.enable_irq();
    
    //recend trigger signal
    trigger = 1;
    wait_us(10);
    trigger = 0;
    
    //restart measurement and reset timeout
    timeout.detach();
    timeout.attach(callback(this,&hcsr04::callback_timeout),HCSR04_TIMEOUT);
}

void hcsr04::callback_rising()
{
    //timeout.detach();
    timer.reset();
    timer.start();
}

void hcsr04::callback_falling()
{
    timeout.detach();
    timer.stop();
    distance = (timer.read_us()) * 0.5 * 0.00034;
    timer.reset();
    
    //restart measurement
    timeout.attach(callback(this,&hcsr04::callback_timeout),HCSR04_TIMEOUT);
}
