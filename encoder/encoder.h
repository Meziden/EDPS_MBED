#ifndef ENCODER_H
#define ENCODER_H

class encoder : public InterruptIn
{
    public:
    encoder(PinName int_pin);
    void start();
    int read();
    void reset();
    
    private:
    void callback_rising();
    void callback_falling();
    
    volatile bool state;
    int sum;
};

#endif
