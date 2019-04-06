#include "mbed.h"
#include "joystick424.h"

joystick424::joystick424(PinName pin_x, PinName pin_y, PinName pin_k): m_stick_x(pin_x),
                                                                        m_stick_y(pin_y),
                                                                        m_stick_k(pin_k)
{
    x=y=0;
    k=0;
    m_flag = 0;
    m_flag_on = 1;
    attach(callback(this,&joystick424::_refresh), FRAMETIME);
}

void joystick424::_refresh(void)
{
    x = m_stick_x.read();
    y = m_stick_y.read();
    k = !m_stick_k;
    
    if(m_flag_on)
    {
        if(x > 0.5 + FLAG_THRESH)
        {
            m_flag |= FLAG_X_POS;
            _disable_flag();
            m_timeout.attach(callback(this,&joystick424::_enable_flag),FLAG_DELAY);
        }
        if(x < 0.5 - FLAG_THRESH)
        {
            m_flag |= FLAG_X_NEG;
            _disable_flag();
            m_timeout.attach(callback(this,&joystick424::_enable_flag),FLAG_DELAY);
        }
        if(y > 0.5 + FLAG_THRESH)
        {
            m_flag |= FLAG_Y_POS;
            _disable_flag();
            m_timeout.attach(callback(this,&joystick424::_enable_flag),FLAG_DELAY);
        }
        if(y < 0.5 - FLAG_THRESH)
        {
            m_flag |= FLAG_Y_NEG;
            _disable_flag();
            m_timeout.attach(callback(this,&joystick424::_enable_flag),FLAG_DELAY);
        }
        if(k)
        { 
            m_flag |= FLAG_K_PRESS;
            _disable_flag();
            m_timeout.attach(callback(this,&joystick424::_enable_flag),FLAG_DELAY);
        }
    }
}

int joystick424::getflag(int flag_type)
{
    int flag_tmp = flag_type & m_flag;
    if(flag_tmp)    // Flag is TRUE now
    {
        // Remove Flag
        m_flag = m_flag & (~flag_type);
        // Return True
        return 1;
    }
    else
        return 0;
}

void joystick424::resetflag()
{
    m_flag = 0;
}

void joystick424::_disable_flag()
{
    m_flag_on = 0;
}

void joystick424::_enable_flag()
{
    m_flag_on = 1;
}
