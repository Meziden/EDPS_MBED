#ifndef HCSR04_H
#define HCSR04_H

//0.05sec x 340m/s x0.5 = 8.5m
const float HCSR04_TIMEOUT = 0.05; 

class hcsr04
{
    public:
    //Constructor
    hcsr04(PinName trig_pin, PinName recv_pin);
    //get distance, in meters.
    float getdistance();
    
    private:
    void callback_rising();
    void callback_falling();
    void callback_timeout();
    
    float distance;
    DigitalOut trigger;
    InterruptIn receiver;
    Timer timer;
    Timeout timeout;
};

#endif
