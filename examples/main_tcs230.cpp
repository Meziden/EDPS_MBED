#include "mbed.h"
#include "tcs230.h"

Serial usbport(USBTX,USBRX);
tcs230 color_detector(p5,p6,p7,p8,p9);

int main()
{
    usbport.baud(9600);
    usbport.format(8,SerialBase::None,1);
    while(1)
    {
        int buffer[3] = {0};
        color_detector.getraw(buffer);
        usbport.printf("R:%d G:%d B:%d\n", buffer[0], buffer[1],buffer[2]);
        wait(1);
    }
    return 0;
}
