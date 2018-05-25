//Data-only psensor encapsulation v2.0

#ifndef PSENSOR_H
#define PSENSOR_H

const int BUFFER_SIZE = 20;
const int CALIB_POSITION_MAX = 1;
const int CALIB_POSITION_MIN = 2;

const float GLOVE_POSITION_MIN = 0.15;
const float GLOVE_POSITION_MAX = 0.35;

const float SPEED_LIMIT = 0.35;
const float CALIB_SPEED_LIMIT = 0.35;

class psensor
{
    public:
        //Constructor
        psensor();
        float getspeed();
        float getspeed_glove();
        
        //Calibrate APIs
        int calib_pressure();
        int calib_position(int);
        
        //Glove Calibrate APIs
        int setposlim1(float);
        int setposlim2(float);
        
        float getspeed_poscalib();
        
        //DEBUG APIs
        float getvol(int);
        float getposition();
        
        int write(float vol1_tmp, float vol2_tmp, float position_tmp, float position_target_tmp);
        
    private:
        float position;
        float position_target;
        //store sample data
        float buffer1[BUFFER_SIZE];
        float buffer2[BUFFER_SIZE];
        
        //channel 1 and 2's offset.(Channel 1:bend,pos-,Channel 2:extend,pos+)
        float offset1,offset2;
        //channel 1 and 2's speed limit.
        float speedlim1,speedlim2;
        //channel 1 and 2's position limit.(poslim2 > poslim1)
        float poslim1,poslim2;
};

#endif
