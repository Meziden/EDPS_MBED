#include "mbed.h"
#include "max7219.h"

//Overall Timer
int Clock=0;
const int FrameDelay = 31;//32Hz Sampling Rate

//Pulse Rate
int PulseRate=0;

//Analog Input Port
AnalogIn Input(p15);

//Other ports
DigitalIn Button(p10);
DigitalOut Dec_power(p7);

//Voltage data arrays
float VolArr[64]       ={0};
float VolArr_tmp[64]   ={0};

//Pulse Checking Threshold (voltage level (from read();) change in 1/16 second)
//0.05 for 165*16mV/sec
const float Threshold = 0.05;

int pulse_analyze(const float* VolArr);//Prototype of Pulse Analyze

int main()
{
    Dec_power=1;
    wait_ms(100);
    max7219 screen(p11,p12,p13,p6);
    while(1)
    {
        //Display
        if( (Clock%4) ==0)
        {
            if(Button)
                screen.graph(VolArr,270);
            else
                screen.graph(PulseRate,270);
        }
        
        //Voltage Cache refreshing
        for(int i=0;i<63;i++)
            VolArr_tmp[i] = VolArr[i+1];//left-shift in tmp cache
        VolArr_tmp[63] = Input.read();//Record New Data
        for(int i=0;i<64;i++)
            VolArr[i] = VolArr_tmp[i];
            
        //
        if( (Clock%32) == 0 )
        PulseRate = pulse_analyze(VolArr);
        
        //+1s
        Clock++;
        wait_ms(FrameDelay);
    }
    return 0;
}

int pulse_analyze(const float* VolArr)//Rising Edge Check
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
