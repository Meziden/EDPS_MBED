// WARNING: This is only a simple, naive and uncompleted encapsulation for complex in ARM mbed.
// If you got any problem, please email Zhang Siming(zhangsiming1724@hotmail.com) for more information. 
#ifndef COMPLEX_H
#define COMPLEX_H

class complex
{
    public:
    // Default Constructor
    complex();
    // Another Constructor
    complex(float arg_real, float arg_imag = 0.0f);
    // Copy Constructor
    complex(const complex &obj);
    
    // The most important data.
    float real;
    float imag;
    
    // Operator Overloads
    complex& operator=(const complex &obj);
    complex operator+(const complex &obj);
    complex operator-(const complex &obj);
    complex operator-();
    
    complex operator*(const complex &obj);
    complex operator/(const complex &obj);
    
    // External functions
    complex conj();
    float abs();
    float angle();
};

complex exp(const complex &obj);

#endif
