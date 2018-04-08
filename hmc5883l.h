//HMC5883L 3-axis magnetometer encapsulation

#ifndef HMC5883L_H
#define HMC5883L_H

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
    
    private:
    float offset_x, offset_y, offset_z;
    float scale_x, scale_y, scale_z;
    
};



#endif
