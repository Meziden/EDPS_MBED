//Data-only exskl_glove encapsulation v2.0

#ifndef EXSKL_GLOVE_H
#define EXSKL_GLOVE_H

const int BUFFER_SIZE = 20;
const float SPEED_LIMIT = 0.35;

const float SPEEDLIM_RANGE = 0.03;
const float POSLIMIT_MIN = 0.15;
const float POSLIMIT_MAX = 0.35;

const float FIR_ARGS[5] = {0.5,0.25,0.125,0.0625,0.03125};

class exskl_glove
{
    public:
        //Constructor
        exskl_glove();
        float getspeed();
        
        //DEBUG APIs
        float getposition();
        
        int write(float position_exskl,float position_glove);
        
    private:
        float buffer_exskl[BUFFER_SIZE];
        float buffer_glove[BUFFER_SIZE];
};

#endif
