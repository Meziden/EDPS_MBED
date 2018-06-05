#include "mbed.h"
#include "encoder.h"

encoder wheelcount1(p8);
Serial usbport(USBTX,USBRX);

int main()
{
    usbport.format(8,SerialBase::None,1);
    usbport.baud(9600);
    wheelcount1.start();
    while(1)
    {
        usbport.printf("%d\n",wheelcount1.read());
        wait(1);
    }
    return 0;
}
