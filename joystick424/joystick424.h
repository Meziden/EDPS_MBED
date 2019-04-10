#ifndef JOYSTICK_424_H
#define JOYSTICK_424_H

const float FRAMETIME = 0.02; //50Hz Refresh Rate
const float FLAG_THRESH = 0.25;

const float FLAG_DELAY = 0.39;
const int FLAG_X_POS = 0x01;
const int FLAG_X_NEG = 0x02;
const int FLAG_Y_POS = 0x04;
const int FLAG_Y_NEG = 0x08;
const int FLAG_K_PRESS = 0x10;


class joystick424
{
    public:
    joystick424(PinName pin_x, PinName pin_y, PinName pin_k);
    
    // Direct Access, Enough for Common Use:
    float x;
    float y;
    bool k;
    
    // Disable this when using ananlog read, 'casue AnalogIn::read() is not interrupt safe.
    bool m_analog_ctl;
    void m_refresh(void);
    int getflag(int flag_type);
    void resetflag();
    
    private:
    AnalogIn m_stick_x;
    AnalogIn m_stick_y;
    DigitalIn m_stick_k;
    
    int m_flag;
    
    int m_flag_on;
    Timeout m_timeout;
    void _enable_flag();
    void _disable_flag();
};

#endif
