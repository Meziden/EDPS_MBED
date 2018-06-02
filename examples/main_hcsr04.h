#include "mbed.h"
#include "hcsr04.h"

Serial usbport(USBTX,USBRX);
hcsr04 ultrasonic(p6,p5);

int main()
{
    usbport.baud(9600);
    usbport.format(8,SerialBase::None,1);
    
    while(1)
    {
        float distance_tmp = ultrasonic.getdistance();
        if(distance_tmp > 0)
            usbport.printf("HC-SR04 DISTANCE: %.2f\n",distance_tmp);
        else
            usbport.printf("HC-SR04 OUT OF RANGE: %.2f\n",distance_tmp);
        wait(0.2);
    }
}
