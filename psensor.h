//psensor encapsulation

#ifndef PSENSOR_H
#define PSENSOR_H

const int BUFFER_SIZE = 20;
const int CALIB_POSITION_MAX = 1;
const int CALIB_POSITION_MIN = 2;

class psensor
{
    public:
        //Constructor
        psensor(PinName, PinName, PinName);
        float getspeed();
        
        //Calibrate APIs
        int calib_pressure();
        int calib_position(int);
        float getspeed_poscalib();
        
        //DEBUG APIs
        float getvol(int);
        float getposition();
    
    private:
        int update();
        AnalogIn input1;
        AnalogIn input2;
        AnalogIn position;
        
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
