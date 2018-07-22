#include "mbed.h"
#include "ds1307.h"

ds1307::ds1307(PinName sda_pin,PinName scl_pin):I2C(sda_pin,scl_pin)
{
    for(size_t i = 0; i != 8; i++)
        buffer[i] = 0;
    frequency(100000);
    update();
}

int ds1307::update()
{
    //update buffer
    start();
    write(0xD0);       //reset pointer
    write(0x00);
    stop();
    
    wait_ms(1);
    
    start();
    write(0xD1);        //read mode
    for(size_t i=0; i != 7; i++)
        buffer[i] = read(1);
    buffer[6] = read(0);
    stop();
    
    //convert buffer to date
    date.m_second = 10 * ((buffer[0] & 0x70) >> 4) + (buffer[0] & 0x0F);
    date.m_minute = 10 * ((buffer[1] & 0x70) >> 4) + (buffer[1] & 0x0F);
    date.m_hour   = 10 * ((buffer[2] & 0x10) >> 4) + (buffer[2] & 0x0F);
    date.m_day    = buffer[3] & 0x07;
    date.m_date   = 10 * ((buffer[4] & 0x30) >> 4) + (buffer[4] & 0x0F);
    date.m_month  = 10 * ((buffer[5] & 0x01) >> 4) + (buffer[5] & 0x0F);
    date.m_year   = 10 * (buffer[6] >> 4) + (buffer[6] & 0x0F) + 2000;
    
    if(buffer[0] & 0x80 == 0)
        return 0;
    else
        return -1;
}

int ds1307::conf(uint8_t clock_flag)
{
    start();
    write(0xD0);
    write(0x00);
    write(clock_flag);
    stop();
    return 0;
}
