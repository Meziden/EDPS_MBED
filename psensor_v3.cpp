#include "mbed.h"
#include "psensor_v2.h"

psensor::psensor()
{
    speedlim1 = speedlim2 = SPEED_LIMIT;
    offset1 = offset2 = 0.0;
    poslim1 = 1.0;
    poslim2 = 0.0;
    position = 0.0;
    position_target = 0.0;
    for(size_t i = 0; i != BUFFER_SIZE; i++)
        buffer1[i] = buffer2[i] = 0;
}

float psensor::getspeed()
{
    float vol1 = 0, vol2 = 0;
    for(size_t i = 0; i < 4; i++)
    {
        vol1 += (buffer1[i]/4);
        vol2 += (buffer2[i]/4);
    }
    
    //PRE-PROCESS
    vol1 = vol1 - offset1;
    vol2 = vol2 - offset2;
    
    if(vol1 < 0)
        vol1 = 0.0;

    if(vol1 > speedlim1)
        vol1 = speedlim1;
        
    if(vol2 < 0)
        vol2 = 0.0;
                
    if(vol2 > speedlim2)
        vol2 = speedlim2;

    //CONTROL LOGIC ------------------------------------------------------------
    float speed_tmp = 0.0;
    
    if((poslim1 < position) && (position < poslim2))        //In range
    {
        float distance = poslim2 - poslim1;                 //watch out, zero-division exception if not in range
        
        if(vol1 >= 0.03)
            speed_tmp = vol1 * sqrt((position - poslim1) / distance); 
        //Override speed_tmp for vol2's higher priority
        if(vol2 >= 0.02)
            speed_tmp = -vol2 * sqrt((poslim2 - position) / distance);
    }
    //CONTROL LOGIC END --------------------------------------------------------
    
    return speed_tmp;
}

float psensor::getspeed_glove()
{
    //CONTROL LOGIC ------------------------------------------------------------
    float speed_tmp = 0.0;
    
    if((poslim1 < position) && (position < poslim2))        //In range
    {
        float distance = poslim2 - poslim1;
        if(position_target > position)
            speed_tmp = - sqrt((position_target - position) / distance);
        else
            speed_tmp = sqrt((position - position) / distance);
        
        if(speed_tmp > SPEED_LIMIT)
            speed_tmp = SPEED_LIMIT;
        if(speed_tmp < -SPEED_LIMIT)
            speed_tmp = -SPEED_LIMIT;
        
    }
    //CONTROL LOGIC END --------------------------------------------------------
    
    return speed_tmp;
}

int psensor::calib_pressure()
{
    float error1 = 0.0;
    float error2 = 0.0;
    
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
        poslim1 = position;
    if (posnum == CALIB_POSITION_MAX)
        poslim2 = position;
    return 0;
}

float psensor::getspeed_poscalib()
{
    float vol1 = 0, vol2 = 0;
    for(size_t i = 0; i < 4; i++)
    {
        vol1 += (buffer1[i]/4);
        vol2 += (buffer2[i]/4);
    }
    
    //PRE-PROCESS
    vol1 = vol1 - offset1;
    vol2 = vol2 - offset2;
    
    if(vol1 < 0)
        vol1 = 0.0;

    if(vol1 > CALIB_SPEED_LIMIT)
        vol1 = CALIB_SPEED_LIMIT;
        
    if(vol2 < 0)
        vol2 = 0.0;
                
    if(vol2 > CALIB_SPEED_LIMIT)
        vol2 = CALIB_SPEED_LIMIT;
    
    float speed_tmp = 0.0;
    
    //UNPROTECTED SPEED SCHEDULING:
    if(vol1 >= 0.03)
        speed_tmp = vol1; 
    if(vol2 >= 0.02)                                    //vol2 higher priority
        speed_tmp = -vol2;                              //override speed if vol2 >= 0.02
    
    return speed_tmp;
}

//TODO
float psensor::getspeed_poscalib_glove()
{
    float speed_tmp = 0;
    
    //UNPROTECTED SPEED SCHEDULING
    if(position_target - position > 0.02)
        speed_tmp = -CALIB_SPEED_LIMIT;
    if(position_target - position < -0.02)
        speed_tmp = CALIB_SPEED_LIMIT;
    
    return speed_tmp;
}

//DEBUG APIs
float psensor::getvol(int channel)
{
    switch(channel)
    {
        case 1 : return buffer1[0]; 
        case 2 : return buffer2[0];
    }
    return 0.0;
}

float psensor::getposition()
{
    return position;
}

int psensor::write(float vol1_tmp, float vol2_tmp,float position_tmp,float position_target_tmp)
{
    position = position_tmp;
    position_target = position_target_tmp;
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
    buffer1_tmp[0] = vol1_tmp;
    buffer2_tmp[0] = vol2_tmp;
    //rewrite buffer
    for(size_t i = 0; i != BUFFER_SIZE; i++)
    {
        buffer1[i] = buffer1_tmp[i];
        buffer2[i] = buffer2_tmp[i];
    }
    return 0;
}
