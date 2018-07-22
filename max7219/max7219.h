#ifndef MAX7219_H
#define MAX7219_H

class max7219:public SPI
{
    public:
    //Default constructor
    max7219();
    //Overloaded constructor
    max7219(PinName MOSI,PinName MISO,PinName SCLK,PinName CS);
    //Init
    int init();
    //MAX7219 SPI PORT
    int write16(int addr,int data);
    //Graph for numbers
    int graph(int number,int degree=0);
    //Graph for Voltage Array
    int graph(const float* VolArr,int degree=0);
    
    private:
    
    int rotate(int *FrameBuffer,int degree);
    
    DigitalOut Load;
};

#endif
