//HMC5883L 3-axis magnetometer encapsulation

#ifndef HMC5883L_H
#define HMC5883L_H

const float CALIB_OFFSET_X  = 10 / 2048.0;
const float CALIB_OFFSET_Y  = 0 / 2048.0;
const float CALIB_OFFSET_Z  = 25 / 2048.0;

const uint8_t GAIN_GAUSS_1370   = 0x00;
const uint8_t GAIN_GAUSS_1090   = 0x20;     //Default Configuartion
const uint8_t GAIN_GAUSS_820    = 0x40;
const uint8_t GAIN_GAUSS_660    = 0x60;
const uint8_t GAIN_GAUSS_440    = 0x80;
const uint8_t GAIN_GAUSS_390    = 0xA0;
const uint8_t GAIN_GAUSS_330    = 0xC0;
const uint8_t GAIN_GAUSS_230    = 0xE0;

const uint8_t BIAS_NONE         = 0x00;     //Default Configuration
const uint8_t BIAS_POSITIVE     = 0x01;
const uint8_t BIAS_NEGATIVE     = 0x02;

const uint8_t MODE_SINGLE       = 0x01;     //Default Configuration
const uint8_t MODE_STREAM       = 0x00;
const uint8_t MODE_HANG         = 0x03;

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
