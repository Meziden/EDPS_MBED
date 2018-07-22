#include "mbed.h"
#include "imujy61.h"

imujy61::imujy61(PinName tx_pin, PinName rx_pin) : Serial(tx_pin,rx_pin)
{
    //setting up format
    baud(115200);
    format(8,SerialBase::None,1);
    
    //clear buffer
    for(size_t i = 0; i != 11; i++)
        buffer[i] = 0;
    for(size_t i = 0; i != 3; i++)
    {
        angle[i] = 0;
        aspeed[i] = 0;
        accel[i] = 0;
    }
    
    rcvd_count = 0;
    packet_ready = 0;
    
    //asynchronously - read buffer.
    attach(callback(this,&imujy61::callback_rcvd));
}

void imujy61::callback_rcvd()
{
    buffer[rcvd_count] = getc();
    if(rcvd_count == 0 && buffer[0] != 0x55)
        return;
    rcvd_count++;
    
    if(rcvd_count == 11)
    {
        packet_ready = 1;
        rcvd_count = 0;
    }
    
    if(packet_ready == 1)
    {
        packet_ready = 0;
        if(buffer[0] == 0x55)
        {
            switch(buffer[1])
            {
                case 0x51 :
                accel[AXIS_X] = (short(buffer[3] << 8 | buffer[2])) / 32768.0 * 16;
                accel[AXIS_Y] = (short(buffer[5] << 8 | buffer[4])) / 32768.0 * 16;
                accel[AXIS_Z] = (short(buffer[7] << 8 | buffer[6])) / 32768.0 * 16;
                break;
                case 0x52 :
                aspeed[AXIS_X] = (short(buffer[3] << 8 | buffer[2])) / 32768.0 * 2000;
                aspeed[AXIS_Y] = (short(buffer[5] << 8 | buffer[4])) / 32768.0 * 2000;
                aspeed[AXIS_Z] = (short(buffer[7] << 8 | buffer[6])) / 32768.0 * 2000;
                break;
                case 0x53 :
                angle[AXIS_X] = (short(buffer[3] << 8 | buffer[2])) / 32768.0 * 180;
                angle[AXIS_Y] = (short(buffer[5] << 8 | buffer[4])) / 32768.0 * 180;
                angle[AXIS_Z] = (short(buffer[7] << 8 | buffer[6])) / 32768.0 * 180;
                break;
            }
        }
    }
}

float imujy61::getangle()
{
    return angle[AXIS_Z];
}
