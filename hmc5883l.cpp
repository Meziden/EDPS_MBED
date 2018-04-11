#include "mbed.h"
#include "hmc5883l.h"

hmc5883l::hmc5883l(PinName sda_pin, PinName scl_pin):I2C(sda_pin,scl_pin),
                                                     offset_x(CALIB_OFFSET_X),
                                                     offset_y(CALIB_OFFSET_Y),    //Y-AXIS unused
                                                     offset_z(CALIB_OFFSET_Z),
                                                     scale_x(CALIB_SCALE_X),
                                                     scale_y(CALIB_SCALE_Y),      //Y-AXIS unused
                                                     scale_z(CALIB_SCALE_Z)
{
    //Device Initialization
    frequency(100000);          //100kHz IIC bus
    start();
    write(0x3c);                //device write
    write(0x00);                //write to 0x00
    write(0x78);                //speed configuration
    stop();
    
    start();
    write(0x3c);                //device write
    write(0x01);                //write to 0x00
    write(GAIN_GAUSS_1090 & 0xE0);   //range configuration
    stop();
    
    start();
    write(0x3c);
    write(0x02);                 
    write(0x00);                //continuous measuring mode
    stop();
}

float hmc5883l::getangle(int getcalibrated)
{
    //Send Data Request
    float angle = 0;
    uint8_t buffer[6] = {0};
    
    start();
    write(0x3c);
    write(0x03);                //reset pointer
    stop();
    
    start();
    write(0x3d);                //read operation
    for(size_t i=0; i<5;i++)
        buffer[i] = read(1);    //read with ACK
    buffer[5] = read(0);        //read without ACK
    stop();
    
    //Data Processing
    float x_tmp = (int16_t)((buffer[0] << 8) + buffer[1]) / 2048.0;
    float y_tmp = (int16_t)((buffer[2] << 8) + buffer[3]) / 2048.0;
    float z_tmp = (int16_t)((buffer[4] << 8) + buffer[5]) / 2048.0;
    //calibrate
    if(getcalibrated)
    {
        x_tmp = (x_tmp - offset_x) * scale_x;
        y_tmp = (y_tmp - offset_y) * scale_y;
        z_tmp = (z_tmp - offset_z) * scale_z;
    }
    
    angle = atan2(z_tmp,x_tmp)*(180.0/3.14159265)+180.0;
    
    return angle;
}

void hmc5883l::getraw(uint8_t* buffer)
{
    start();
    write(0x3c);
    write(0x03);                //reset pointer
    stop();
    
    start();
    write(0x3d);                //read operation
    for(size_t i=0; i<5;i++)
        buffer[i] = read(1);    //read with ACK
    buffer[5] = read(0);        //read without ACK
    stop();
}

void hmc5883l::setoffset(float arg_x,
                         float arg_y,
                         float arg_z)
{
    offset_x = arg_x;
    offset_y = arg_y;
    offset_z = arg_z;
}
    
void hmc5883l::setscale(float arg_x,
                        float arg_y,
                        float arg_z)
{
    scale_x = arg_x;
    scale_y = arg_y;
    scale_z = arg_z;
}

void hmc5883l::setgain(int gainflag)
{
    start();
    write(0x3c);                //device write
    write(0x01);                //write to 0x00
    write(gainflag & 0xE0);   //range configuration
    stop();
}
