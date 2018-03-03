//16-1 Analog Multiplexer Encapsulation for mbed

class amux16
{
	public:
	//constructor
	amux16(PinName lsb, PinName pin2, PinName pin3, PinName msb);
	
	//read single port
	float read(const int& channel);
	
	private:
	BusOut port_select;
	AnalogIn port_analog;
};
