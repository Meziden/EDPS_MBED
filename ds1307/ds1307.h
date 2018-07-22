#ifndef DS1307_H
#define DS1307_H

const uint8_t DS1307_LAUNCH  = 0x00;
const uint8_t DS1307_STOP    = 0x80;

typedef struct
{
    uint8_t m_second;
    uint8_t m_minute;
    uint8_t m_hour;
    uint8_t m_day;
    uint8_t m_date;
    uint8_t m_month;
    int m_year;
}mydate_t;

class ds1307 : public I2C
{
    public:
    ds1307(PinName sda_pin,PinName scl_pin);
    int update();
    int conf(uint8_t clock_flag);
    
    mydate_t date;
    
    private:
    uint8_t buffer[8];
};
#endif
