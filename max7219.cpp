#include "mbed.h"
#include "max7219.h"

//Configration for VOlArr Scanning
const int ScanStart = 0;
const int ScanEnd   = 7;
const int ScanSkip  = 1;


#ifndef NUMBER_FONT
#define NUMBER_FONT

//NUMCACHE POWERED BY BIG BROTHER
int numcache[10][8]={   0x00,0x0E,0x0A,0x0A,0x0A,0x0A,0x0E,0x00,    //0
                        0x00,0x04,0x04,0x04,0x04,0x06,0x04,0x00,    //1
                        0x00,0x0E,0x02,0x04,0x08,0x0A,0x04,0x00,    //2
                        0x00,0x06,0x0A,0x08,0x04,0x08,0x0E,0x00,    //3
                        0x00,0x08,0x08,0x08,0x0E,0x0A,0x0A,0x00,    //4
                        0x00,0x06,0x0A,0x08,0x06,0x02,0x0E,0x00,    //5
                        0x00,0x0E,0x0A,0x0A,0x0E,0x02,0x0E,0x00,    //6
                        0x00,0x02,0x02,0x02,0x04,0x08,0x0E,0x00,    //7
                        0x00,0x0E,0x0A,0x0A,0x0E,0x0A,0x0E,0x00,    //8
                        0x00,0x0E,0x0A,0x08,0x0E,0x0A,0x0E,0x00};   //9

#endif

//Default Constructor
max7219::max7219():SPI(p11,p12,p13),Load(p6)
{
    init();
}

//Overloaded Constructor
max7219::max7219(PinName MOSI,PinName MISO,PinName SCLK,PinName CS):SPI(MOSI,MISO,SCLK),Load(CS)
{
    init();
}

//Initialization
int max7219::init()
{
    format(8,0);
    frequency(1000000);
    wait_ms(500);
    
    //Initial Configuration
    write16(0x09,0x00);
    write16(0x0A,0x08);
    write16(0x0B,0x07);
    write16(0x0C,0x01);
    write16(0x0F,0x0F);
    wait_ms(500);
    
    //purge preset data
    for(unsigned int i=0;i<8;i++)
        write16(i+1,0x00);
    
    //Normal Operation Mode
    write16(0x0F,0x00);
    return 0;
}

//write16
int max7219::write16(int addr,int data)
{
    Load=0;             //Slave Select
    SPI::write(addr);   //MSB first
    SPI::write(data);   //LSB
    Load=1;             //Slave Release
    return data;
}

int max7219::graph(const float* VolArr)
{
    int FrameBuffer[8]={0};
    for(int limit=7;limit>=0;limit--)
    {
        for(int pos=ScanStart;pos<ScanEnd;pos+=ScanSkip)
        {
            //samples in last second
            if( VolArr[pos]*8.0 > limit )
                FrameBuffer[limit] |= ( 1 << ((pos-ScanStart)/ScanSkip) );
        }
    }
    for(int i=0;i!=8;i++)
        write16(i+1,FrameBuffer[i]);
    return 0;
}

int max7219::graph(int number)
{
    if(number > 99)
    {
        number=99;
    }
    int FrameBuffer[8]={0};
    for(int i=0;i<8;i++)
    {
        FrameBuffer[i]  = (numcache[number%10][i])<<4;// "<<4" move all numbers to the right half
        FrameBuffer[i] |= (numcache[number/10][i]);
    }
    for(int i=0;i!=8;i++)
        write16(i+1,FrameBuffer[i]);
    return 0;
}
