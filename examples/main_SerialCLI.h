#include "mbed.h"
#include "SerialCLI.h"

DigitalOut leds[3] = {  DigitalOut(LED1),
                        DigitalOut(LED2),
                        DigitalOut(LED3)};
SerialCLI cli(USBTX, USBRX);

int led_func(int argc, char** argv)
{
    if(argc == 2)
    {
        int led_num = atoi(argv[1]) - 1;
        if(led_num > 2 || led_num < 0)
        {
            cli.printf("[Error] Out of range.\n");
            return -1;
        }
        else
            leds[led_num] = !leds[led_num];
    }
    else
        cli.printf("Usage: led LED_NUMBER\n");
    return 0;
}

int main()
{
    cli.add_function("led", led_func);
    
    while(1)
        wait(10);

    return 0;
}
