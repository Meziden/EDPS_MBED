#include "mbed.h"

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

SPI Output(p11,p12,p13);
DigitalOut ss(p6);
DigitalIn button(p10);
DigitalOut dec_power(p7);
AnalogIn analog_port(p15);

int counter = 1;
int pulserate_fb = 0;

//32 samples in 2 seconds
float VolArr[64] = {0};       //Cache for pulse analyzing
float VolArr_tmp[64] = {0};   //Cache for FrameBuffer
const float Threshold = 0.05;

const int FrameDelay = 31; //Frame Delay. 32Hz Sampling Rate 
unsigned char FrameBuffer [8] = {0x00};

int pulse_analyze(const float* VolArr)//Rising Edge Checking
{
    int pulserate=0;
    int start=0,end=0;//Two time stamps.
    while( (VolArr[start+2] - VolArr[start]) < Threshold && start < 51)
        start++;
    end=start+10;
    while( (VolArr[end+2] - VolArr[end]) < Threshold && end < 62)
        end++;
    pulserate=(1920.0/(end-start));
    return pulserate;
}

void write16(int addr,int data)
{
    ss=0;
    Output.write(addr);
    Output.write(data);
    ss=1;
}

int main()
{
    //Initialize the ports
    Output.format(8,0);
    Output.frequency(1000000);
    dec_power=1;
    ss=1;
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
    
    while(1)
    {    
        //Converting voltage cache to frame buffer every two cycle
        //8 frames per second under default settings
        if(counter % 4 == 1)
        {
            if(button)
            {
                for(int limit=7;limit>=0;limit--)
                {
                    FrameBuffer[limit]=0;
                    for(int pos=35;pos<64;pos+=4)
                    {
                        //samples in last second
                        if( VolArr[pos]*8.0 > limit )
                            FrameBuffer[limit] |= ( 1 << ((pos-35)/4) );
                    }
                }
            }
            else
            {
                for(int j=0;j<8;j++)
                    FrameBuffer[j]  = 0;
                if(pulserate_fb>99)
                    pulserate_fb=99;
                for(int i=0;i<8;i++)
                {
                    FrameBuffer[i]  = (numcache[pulserate_fb%10][i])<<4;// "<<4" move all numbers to the right half
                    FrameBuffer[i] |= (numcache[pulserate_fb/10][i]);
                }
            }
            //Sending Data
            for(int i=0;i<8;i++)
                write16(i+1,FrameBuffer[i]);
         }
         
        //Refresh voltage cache
        for(int i=0;i<63;i++)
            VolArr_tmp[i] = VolArr[i+1];//left-shift in tmp cache
        
        VolArr_tmp[63] = analog_port.read();//Record New Data
        
        for(int i=0;i<64;i++)
            VolArr[i] = VolArr_tmp[i];

        //Start Data Analyzing for every second
        if( (counter%32)==0 )
            pulserate_fb = pulse_analyze(VolArr);
        
        wait_ms(FrameDelay);
        counter++;
    }
    return 0;
}
