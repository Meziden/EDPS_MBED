//Encapsulation for MAX7219+1088As module.
//WARNING:BIG BROTHER IS WATCHING YOU.
#ifndef MAX7219_H
#define MAX7219_H

class max7219:public SPI
{
    public:
    //Default constructor
    max7219();
    //Overloaded constructor
    max7219(PinName MOSI,PinName MISO,PinName SCLK,PinName CS);
    //Init,because there's no completed c++11 support in mbed now.
    int init();
    //MAX7219 SPI port data writing
    int write16(int addr,int data);
    //Graph for numbers
    int graph(int number);
    //Graph for Voltage Array
    int graph(const float* VolArr);
    
    private:
    
    DigitalOut Load;
};

#endif
