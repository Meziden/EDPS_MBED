#ifndef IMUJY61_H
#define IMUJY61_H

const size_t AXIS_X = 0;
const size_t AXIS_Y = 1;
const size_t AXIS_Z = 2;

class imujy61 : public Serial
{
    public:
    imujy61(PinName tx_pin, PinName rx_pin);
    float getangle();
    
    uint8_t buffer[11];
    
    float accel[3];
    float aspeed[3];
    float angle[3];
    
    private:
    void callback_rcvd();
    size_t rcvd_count;
    volatile bool packet_ready;
};

#endif
