#include "mbed.h"
#include "tcs230.h"

//Constructor
tcs230::tcs230( PinName fscale1_pin,
                PinName fscale2_pin,
                PinName diodectl1_pin,
                PinName diodectl2_pin,
                PinName recv_pin):
                fscale1(fscale1_pin),
                fscale2(fscale2_pin),
                diodectl1(diodectl1_pin),
                diodectl2(diodectl2_pin),
                receiver(recv_pin)
{
    for(size_t i = 0; i!=3; i++)
        rgb_buffer[i] = 0;
    
    //50kHz, or whatever the hell.
    fscale1 = 1;
    fscale2 = 1;
    sum = 0;
}

int tcs230::getcolor()
{
    //use getraw, obviously
    getraw(rgb_buffer);
    float rgb_normal[3] = {0};
    for(int i = 0; i!=3 ;i++)
        rgb_normal[i] = rgb_buffer[i] / RGB_CALIB[i];
    
    int maxcolor = COLOR_RED;
    for(int i = 1; i!=3 ;i++)
    {
        if(rgb_normal[i] > rgb_normal[maxcolor])
            maxcolor = i;
    }
    return maxcolor;
}

//Recommended for T-DPS
int tcs230::getcolor_hsv()
{
    //use getraw, obviously
    getraw(rgb_buffer);
    float hsvspace_v = 0, hsvspace_s = 0, hsvspace_h = 0;
    float rgb_normal[3] = {0};
    int maxcolor = COLOR_RED, mincolor = COLOR_RED;
    
    for(int i = 0; i!=3 ;i++)
    {
        rgb_normal[i] = (float)rgb_buffer[i] / (float)RGB_CALIB[i];
        if (rgb_normal[i] > 1)
        {
            rgb_normal[i] = 1;
        } 
    }
    
    for(int i = 1; i!=3 ;i++)
    {
        if(rgb_normal[i] > rgb_normal[maxcolor])
            maxcolor = i;
        if(rgb_normal[i] < rgb_normal[mincolor])
            mincolor = i;
    }
    
    //RGB to HSV conversion
    hsvspace_v = rgb_normal[maxcolor];
    
    hsvspace_s = (hsvspace_v - rgb_normal[mincolor]) / hsvspace_v;
    
    switch(maxcolor)
    {
        case COLOR_RED:
        hsvspace_h = 60 * (rgb_normal[COLOR_GREEN] - rgb_normal[COLOR_BLUE]) / (hsvspace_v - rgb_normal[mincolor]);break;
        case COLOR_GREEN:
        hsvspace_h = 120 + 60 * (rgb_normal[COLOR_BLUE] - rgb_normal[COLOR_RED]) / (hsvspace_v - rgb_normal[mincolor]);break;
        case COLOR_BLUE:
        hsvspace_h = 240 + 60 * (rgb_normal[COLOR_RED] - rgb_normal[COLOR_GREEN]) / (hsvspace_v - rgb_normal[mincolor]);break;
    }
    
    if(hsvspace_h < 0)
        hsvspace_h += 360;
    
    if(hsvspace_h < 40)
        return COLOR_HSV_RED;
    else if(hsvspace_h < 70)
        return COLOR_HSV_YELLOW;
    else if(hsvspace_h < 160)
        return COLOR_HSV_GREEN;
    else if(hsvspace_h < 180)
        return COLOR_HSV_CYAN;
    else if(hsvspace_h < 265)
        return COLOR_HSV_BLUE;
    else if(hsvspace_h < 320)
        return COLOR_HSV_MAGENTA;
    else
        return COLOR_HSV_RED;
}

int tcs230::getraw(int* bufferptr)
{
    //get RED intensity
    diodectl1 = 0;
    diodectl2 = 0;
    
    done = 0;
    sum = 0;
    changed = 0;
    timeout.detach();
    timeout.attach(callback(this, &tcs230::callback_timeout),TCS230_TIMEOUT);
    receiver.rise(callback(this, &tcs230::callback_rising));
    receiver.enable_irq();
    
    //wait for callback to change the done status.
    while(!done)
        continue;
    
    bufferptr[COLOR_RED] = sum;
    
    //get GREEN intensity
    diodectl1 = 1;
    diodectl2 = 1;
    
    done = 0;
    sum = 0;
    timeout.detach();
    timeout.attach(callback(this, &tcs230::callback_timeout),TCS230_TIMEOUT);
    receiver.rise(callback(this, &tcs230::callback_rising));
    receiver.enable_irq();
    
    //wait for callback to change the done status.
    while(!done)
        continue;
    
    bufferptr[COLOR_GREEN] = sum;
    
    //get BLUE intensity
    diodectl1 = 0;
    diodectl2 = 1;
    
    done = 0;
    sum = 0;
    timeout.detach();
    timeout.attach(callback(this, &tcs230::callback_timeout),TCS230_TIMEOUT);
    receiver.rise(callback(this, &tcs230::callback_rising));
    receiver.enable_irq();
    
    //wait for callback to change the done status.
    while(done == 0)
        continue;
    
    bufferptr[COLOR_BLUE] = sum;
    
    return done;
}

void tcs230::callback_timeout()
{
    //unbind rising-edge trigger callback
    receiver.rise(NULL);
    
    if(changed == 1)
        done = 1;
    else
        done = -1;
}

void tcs230::callback_rising()
{
    changed = 1;
    sum ++;
}
