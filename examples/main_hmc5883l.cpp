#include "mbed.h"
#include "hmc5883l.h"

hmc5883l compass(p28,p27);
Serial usbport(USBTX,USBRX);

int main()
{
    usbport.format(8, SerialBase::None, 1);
    usbport.baud(9600);
    wait_ms(100);
    while(true)
    {
        usbport.printf("ANGLE IS NOW %.3f\r\n",compass.getangle());
        wait_ms(200);
    }
    return 0;
}
