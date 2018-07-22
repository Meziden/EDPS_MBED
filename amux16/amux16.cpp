#include "mbed.h"
#include "amux16.h"

amux16::amux16(	PinName lsb,
				PinName pin2,
				PinName pin3,
				PinName msb,
				PinName input):
				port_select(lsb,pin2,pin3,msb),
				port_analog(input){}

float amux16::read(const int& channel)
{
	port_select.write(channel);
	return port_analog.read();
}
