#include "mbed.h"
#include "psensor.h"

psensor::psensor(PinName port1, PinName port2, PinName port3):input1(port1),input2(port2),position(port3)
{
    speedlim1 = speedlim2 = 0.40;
    poslim1 = 0.47;
    poslim2 = 0.64;
    offset1 = offset2 = 1.0;
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

float psensor::getvol(int volnum)
{
    if(volnum == 1)
        return input1.read();
    if(volnum == 2)
        return input2.read();
    return 0;
}

float psensor::getposition()
{
    return position.read();
}

float psensor::getspeed()
{
    update();
    float vol1 = 0, vol2 = 0;
    for(size_t i = 0; i < 4; i++)
    {
        vol1 += (buffer1[i]/4);
        vol2 += (buffer2[i]/4);
    }
    float speed_tmp = 0.0;
    
    //PRE-PROCESS ------------------------
    vol1 = (vol1 - offset1) * 1.6;
    vol2 = (vol2 - offset2) * 2.0;
    
    if(vol1 < 0)
        vol1 = 0.0;

    if(vol1 > speedlim1)
        vol1 = speedlim1;
        
    if(vol2 < 0)
        vol2 = 0.0;
                
    if(vol2 > speedlim2)
        vol2 = speedlim2;
    
    //CONTROL LOGIC ----------------------
    if(vol2 >= 0.02 && position.read() < poslim2)
        speed_tmp = -vol2;
    else
    {
        if(vol1 > 0.03 && position.read() > poslim1)
            speed_tmp = vol1;
        else
            speed_tmp = 0.0;
    }
    return speed_tmp;
}

float psensor::getspeed_poscalib()
{
    update();
    float vol1 = 0, vol2 = 0;
    for(size_t i = 0; i < 4; i++)
    {
        vol1 += (buffer1[i]/4);
        vol2 += (buffer2[i]/4);
    }
    float speed_tmp = 0.0;
    
    //PRE-PROCESS ------------------------
    vol1 = (vol1 - offset1) * 1.6;
    vol2 = (vol2 - offset2) * 2.0;
    
    if(vol1 < 0)
        vol1 = 0.0;

    if(vol1 > speedlim1)
        vol1 = speedlim1;
        
    if(vol2 < 0)
        vol2 = 0.0;
                
    if(vol2 > speedlim2)
        vol2 = speedlim2;
    
    //CONTROL LOGIC ----------------------
    if(vol2 >= 0.02)
        speed_tmp = -vol2;
    else
    {
        if(vol1 > 0.03)
            speed_tmp = vol1;
        else
            speed_tmp = 0.0;
    }
    return speed_tmp;
}

int psensor::calib_pressure()
{
    update();
    float error1 = 0.0;
    float error2 = 0.0;
    
    //absolute summation
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

int psensor::calib_position(int posnum)
{
    if (posnum == CALIB_POSITION_MIN)
        poslim1 = position.read();
    if (posnum == CALIB_POSITION_MAX)
        poslim2 = position.read();
    return 0;
}
