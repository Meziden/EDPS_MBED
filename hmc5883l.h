//HMC5883L 3-axis magnetometer encapsulation

#ifndef HMC5883L_H
#define HMC5883L_H

const float CALIB_OFFSET_X  = 0.09;
const float CALIB_OFFSET_Y  = 0.00;
const float CALIB_OFFSET_Z  = 0.05;

const int GAIN_GAUSS_1370   = 0x00;
const int GAIN_GAUSS_1090   = 0x20;     //Default Configuartion
const int GAIN_GAUSS_820    = 0x40;
const int GAIN_GAUSS_660    = 0x60;
const int GAIN_GAUSS_440    = 0x80;
const int GAIN_GAUSS_390    = 0xA0;
const int GAIN_GAUSS_330    = 0xC0;
const int GAIN_GAUSS_230    = 0xE0;

const int BIAS_NONE         = 0x00;     //Default Configuration
const int BIAS_POSITIVE     = 0x01;
const int BIAS_NEGATIVE     = 0x02;

const int MODE_SINGLE       = 0x01;
const int MODE_STREAM       = 0x00;     //Default Configuration
const int MODE_HANG         = 0x03;

class hmc5883l : public I2C
{
    public:
    //Constructor
    hmc5883l(PinName sda_pin, PinName scl_pin);
    
    float getangle();
    //debug only
    void getraw(uint8_t* bufferptr);
    
    //utils
    void setgain(int gainflag);
    void setbias(int biasflag);
    void setmode(int modeflag);
    
    //calibration related functions
    void setoffset(float arg_x, float arg_y, float arg_z);
    void setscale(float arg_x, float arg_y,float arg_z);
    void calibscale();
    
    private:
    float offset_x, offset_y, offset_z;
    float scale_x, scale_y, scale_z;
};



#endif
