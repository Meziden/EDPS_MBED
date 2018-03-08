#ifndef PSENSOR_H
#define PSENSOR_H

const int BUFFER_SIZE = 20;

class psensor
{
    public:
        //constructor
        psensor(PinName, PinName);
        
        float getspeed();
        int calibrate();
    
    private:
        int update();
        AnalogIn input1;
        AnalogIn input2;
        
        //store sample data
        float buffer1[BUFFER_SIZE];
        float buffer2[BUFFER_SIZE];
        
        float offset1,offset2;
        float limit1,limit2;
};

#endif
