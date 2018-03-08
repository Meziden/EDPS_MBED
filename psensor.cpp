#include "mbed.h"
#include "psensor.h"

psensor::psensor(PinName port1, PinName port2):input1(port1),input2(port2)
{
    limit1 = limit2 = 0.05;
    offset1 = offset2 = 0.0;
    for(size_t i = 0; i != BUFFER_SIZE; i++)
        buffer1[i] = buffer2[i] = 0;
}

int psensor::update()
{
    //temp buffer
    float buffer1_tmp[BUFFER_SIZE] = {0};
    float buffer2_tmp[BUFFER_SIZE] = {0};
    //right-shift
    for(size_t i = 0; i != (BUFFER_SIZE - 1); i++)
    {
        buffer1_tmp[i+1] = buffer1[i];
        buffer2_tmp[i+1] = buffer2[i];
    }
    //insert new sample
    buffer1_tmp[0] = input1.read();
    buffer2_tmp[0] = input2.read();
    //rewrite buffer
    for(size_t i = 0; i != BUFFER_SIZE; i++)
    {
        buffer1[i] = buffer1_tmp[i];
        buffer2[i] = buffer2_tmp[i];
    }
    return 0;
}

float psensor::getspeed()
{
    update();
    float vol1 = buffer1[0];
    float vol2 = buffer2[0];
    float speed_tmp = 0.0;
    
    //PRE-PROCESS
    vol1 = offset1 - vol1;
    vol2 = offset2 - vol2;
    
    if(vol1 < 0)
        vol1 = 0.0;

    if(vol1 > limit1)
        vol1 = limit1;
        
    if(vol2 < 0)
        vol2 = 0.0;
                
    if(vol2 > limit2)
        vol2 = limit2;
    
    //CONTROL LOGIC
    if(vol2 >= 0.02)
        speed_tmp = -vol2;
    
    if(vol1 > 0.03 && vol2 < 0.02)
        speed_tmp = vol1;
    
    if(vol1 <= 0.03 && vol2 < 0.02)
        speed_tmp = 0.0;
    
    return speed_tmp;
}

int psensor::calibrate()
{
    update();
    float error1 = 0;
    float error2 = 0;
    
    for(size_t i = 1; i != BUFFER_SIZE; i++)
    {
        float error1_tmp = buffer1[i] - buffer1[i-1];
        float error2_tmp = buffer2[i] - buffer2[i-1];
        
        if(error1 > 0)
            error1 += error1_tmp;
        else
            error1 -= error1_tmp;
        
        if(error2 > 0)
            error2 += error2_tmp;
        else
            error2 -= error2_tmp;
    }
    
    if(error1 < 0.3 && error2 < 0.3)
    {
        offset1 = buffer1[0];
        offset2 = buffer2[0];
        return 1;
    }
    else
        return 0;
}
