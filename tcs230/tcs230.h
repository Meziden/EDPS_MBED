#ifndef TCS230_H
#define TCS230_H

const float TCS230_TIMEOUT = 0.05;
const int RGB_CALIB[3]  = {2200,1900,2500};//for [1,1] freq configuration.

//DO NOT CHANGE THESE VALUE, OR INVALID ACCESS EXCEPTION HAPPENS
const int COLOR_RED     = 0;
const int COLOR_GREEN   = 1;
const int COLOR_BLUE    = 2;

//THESE CONSTS CAN BE CHANGED, SOME ARE HSV RELATED

const int COLOR_HSV_RED = 0;
const int COLOR_HSV_YELLOW = 1;
const int COLOR_HSV_GREEN = 2;
const int COLOR_HSV_CYAN = 3;
const int COLOR_HSV_BLUE = 4;
const int COLOR_HSV_MAGENTA = 5;

class tcs230
{
    public:
    //Constructor
    tcs230( PinName fscale1_pin,
            PinName fscale2_pin,
            PinName diodectl1_pin,
            PinName diodectl2_pin,
            PinName recv_pin);
    
    int getcolor();
    int getcolor_hsv();
    int getraw(int* buffer);
    
    private:
    Timeout timeout;
    void callback_timeout();
    
    DigitalOut fscale1;
    DigitalOut fscale2;
    DigitalOut diodectl1;
    DigitalOut diodectl2;
    
    InterruptIn receiver;
    volatile int done;
    volatile int sum;
    bool changed;
    void callback_rising();
    
    int rgb_buffer[3];
};

#endif
