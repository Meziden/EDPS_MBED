#include "mbed.h"
#include "hmc5883l.h"

hmc5883l compass(p28,p27);
Serial usbport(USBTX,USBRX);

int main()
{
    uint8_t buffer[6] = {0};
    usbport.format(8, SerialBase::None, 1);
    usbport.baud(9600);
    
    compass.setgain(GAIN_GAUSS_1090);
    compass.setmode(MODE_STREAM);
    
    wait_ms(100);
    while(true)
    {
        compass.getraw(buffer);
        //The output of following code can be  
        usbport.printf("%d,%d,%d\n",(int16_t)((buffer[0]<<8)+buffer[1]),
                                    (int16_t)((buffer[2]<<8)+buffer[3]),
                                    (int16_t)((buffer[4]<<8)+buffer[5]));
        //usbport.printf("ANGLE IS NOW %.3f\r\n",compass.getangle());
        wait_ms(100);
    }
    return 0;
}
