//HMC5883L 3-axis magnetometer encapsulation

#ifndef HMC5883L_H
#define HMC5883L_H

const float CALIB_OFFSET_X = 0.09;
const float CALIB_OFFSET_Y = 0.00;
const float CALIB_OFFSET_Z = 0.05;

const float CALIB_SCALE_X = 1.00;
const float CALIB_SCALE_Y = 1.00;
const float CALIB_SCALE_Z = 0.97;

const int GAIN_GAUSS_1370 = 0x00;
const int GAIN_GAUSS_1090 = 0x02;   //Default Configuartion
const int GAIN_GAUSS_820 = 0x04;
const int GAIN_GAUSS_660 = 0x06;
const int GAIN_GAUSS_440 = 0x08;
const int GAIN_GAUSS_390 = 0x0A;
const int GAIN_GAUSS_330 = 0x0C;
const int GAIN_GAUSS_230 = 0x0E;

class hmc5883l : public I2C
{
    public:
    //Constructor
    hmc5883l(PinName sda_pin, PinName scl_pin);
    
    float getangle(int getcalibrated = 1);
    //debug only
    void getraw(uint8_t* bufferptr);
    
    void setoffset( float arg_x,
                    float arg_y,
                    float arg_z);
    
    void setscale(  float arg_x,
                    float arg_y,
                    float arg_z);
                    
    void setgain(int gainflag);
    
    private:
    float offset_x, offset_y, offset_z;
    float scale_x, scale_y, scale_z;
    
};



#endif
