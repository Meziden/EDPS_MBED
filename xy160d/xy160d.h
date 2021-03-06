#ifndef XY160D_H
#define XY160D_H

const int XY160D_FORWARD    = 0x01;
const int XY160D_BACKWARD   = 0x02;
const int XY160D_HANG       = 0x01 | 0x02;
const int XY160D_BRAKE      = 0;

class xy160d : public PwmOut
{
    public:
        //constructors
        xy160d(PinName pwmpin, PinName ctlpin1, PinName ctlpin2);
        float operator=(const float&);

    private:
        void setmode(int mode_tmp);
        DigitalOut ctl_1;
        DigitalOut ctl_2;
        int mode_present;
};

#endif
