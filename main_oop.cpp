#include "mbed.h"
#include "max7219.h"		//Encapsulation for LED display.


int Clock=0;				//Overall Clock,controls the rendering and data analyzing frequency.
const int FrameDelay = 31;	//used in wait_ms(int FrameDelay),controls the sampling rate. 

int PulseRate=0;			//Pulse Rate

AnalogIn Input(p15);		//Analog Input Port

//Other ports
DigitalIn Button(p10);		//Switch for display modes(numbers/waveform)
DigitalOut Dec_power(p7);	//Power supply for LED display module.

//Voltage data arrays
float VolArr[64]={0};		//storing samples for voltage
float VolArr_tmp[64]={0};	//a backup when refreshing data

//Pulse Checking Threshold (voltage level (from read();) change in 1/16 second)
//0.03 for 1.68 V/sec
const float Threshold = 0.03;

int pulse_analyze(const float* VolArr);	//Prototype of Pulse Analyze

int main()
{
    Dec_power=1;					//Turn on power supply
    wait_ms(100);					//wait for a while
    max7219 screen_lhs(p11,p12,p13,p5);    //Screen 1 (Left Hand Side)
    max7219 screen_rhs(p11,p12,p13,p6);    //Screen 2 (Right Hand Side)
    while(1)
    {
        //Refresh display for every 4 samples.
        if( (Clock%4) ==0)
        {
            if(Button)//decide the display mode.
            {
                screen_lhs.graph(VolArr,270);		//Samples 03-31 on left screen
                screen_rhs.graph(VolArr+32,270);	//Samples 35-64 on right screen
            }
            else
            {
                screen_lhs.graph(PulseRate/100,270);//first two digits with 270 degree,clockwise rotation.
                screen_rhs.graph(PulseRate%100,270);//last two digits with 270 degree,clockwise rotation.
            }
        }
        
        //Voltage Cache refreshing
        for(int i=0;i<63;i++)
            VolArr_tmp[i] = VolArr[i+1];//left-shift in tmp cache,remove the oldest one.
		
        VolArr_tmp[63] = Input.read();//Record new data to the latest position.
		
        for(int i=0;i<64;i++)
            VolArr[i] = VolArr_tmp[i];//use the refreshed data
            
        //analyze data every second
        if( (Clock%32) == 0 )
        	PulseRate = pulse_analyze(VolArr);
        
        //Clock +1,entering next cycle.
        Clock++;
        wait_ms(FrameDelay);
    }
    return 0;
}

int pulse_analyze(const float* VolArr)  //returns the pulserate.
{
    int start=0,end=0;
    while( VolArr[start+2] - VolArr[start] > 0 && start < 62 )
        start++;
    while( VolArr[start+2] - VolArr[start] < Threshold && start < 51 )
        start++;
    end=start+10;
    while( VolArr[end+2] - VolArr[end] > 0 && end < 62 )
        end++;
    while( VolArr[end+2] - VolArr[end] < Threshold && end < 62 )
        end++;
    return 1920.0/(end-start);
}
