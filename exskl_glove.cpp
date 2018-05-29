#include "mbed.h"
#include "exskl_glove.h"

exskl_glove::exskl_glove()
{
    for(size_t i = 0; i != BUFFER_SIZE; i++)
        buffer_exskl[i] = buffer_glove[i] = 0;
}

int exskl_glove::write(float position_exskl_tmp, float position_glove_tmp)
{
    //temp buffer
    float buffer_exskl_tmp[BUFFER_SIZE] = {0};
    float buffer_glove_tmp[BUFFER_SIZE] = {0};
    //right-shift
    for(size_t i = 0; i != (BUFFER_SIZE - 1); i++)
    {
        buffer_exskl_tmp[i+1] = buffer_exskl[i];
        buffer_glove_tmp[i+1] = buffer_glove[i];
    }
    //insert new sample
    buffer_exskl_tmp[0] = position_exskl_tmp;
    buffer_glove_tmp[0] = position_glove_tmp;
    //rewrite buffer
    for(size_t i = 0; i != BUFFER_SIZE; i++)
    {
        buffer_exskl[i] = buffer_exskl_tmp[i];
        buffer_glove[i] = buffer_glove_tmp[i];
    }
    return 0;
}

float exskl_glove::getspeed()
{
    float distance = 0,posfir_glove = 0, posfir_exskl = 0;
    for(size_t i = 0; i < 5; i++)
    {
        posfir_exskl += (buffer_exskl[i] * FIR_ARGS[i]);
        posfir_glove += (buffer_glove[i] * FIR_ARGS[i]);
    }
    
    distance = posfir_glove - posfir_exskl;
    
    float speed_tmp = 0;
    //WARNING: UNTESTED SCHEDULING
    if(distance > 0.02 && posfir_exskl < POSLIMIT_MAX)
    {
        speed_tmp = SPEED_LIMIT;
        if((POSLIMIT_MAX - posfir_exskl) < SPEEDLIM_RANGE)
            speed_tmp *= (POSLIMIT_MAX - posfir_exskl) / SPEEDLIM_RANGE;
    }
    if(distance < -0.02 && posfir_exskl > POSLIMIT_MIN)
    {
        speed_tmp = -SPEED_LIMIT;
        if((posfir_exskl - POSLIMIT_MIN) < SPEEDLIM_RANGE)
            speed_tmp *= (posfir_exskl - POSLIMIT_MIN) / SPEEDLIM_RANGE;
    }
    return speed_tmp;
}

//DEBUG APIs
float exskl_glove::getposition()
{
    return buffer_exskl[0];
}
