#include "mbed.h"
#include "complex.h"

// Default Constructor
complex::complex()
{
    real = imag = 0;
}

// Envaluate Constructor
complex::complex(float arg_real, float arg_imag)
{
    real = arg_real;
    imag = arg_imag;
}

// Copy Constructor
complex::complex(const complex &obj)
{
    real = obj.real;
    imag = obj.imag;
}

// Operator Overloads
complex& complex::operator=(const complex &obj)
{
    real = obj.real;
    imag = obj.imag;
    return *this;
}

complex complex::operator+(const complex &obj)
{
    complex result;
    result.real = real + obj.real;
    result.imag = imag + obj.imag;
    return result;
}

complex complex::operator-(const complex &obj)
{
    complex result;
    result.real = real - obj.real;
    result.imag = imag - obj.imag;
    return result;
}

complex complex::operator-()
{
    complex result;
    result.real = -real;
    result.imag = -imag;
    return result;
}

complex complex::operator*(const complex &obj)
{
    complex result;
    result.real = real * obj.real - imag * obj.imag;
    result.imag = real * obj.imag + imag * obj.real;
    return result;
}

complex complex::operator/(const complex &obj)
{
    complex result;
    result.real = (real * obj.real + imag * obj.imag) / (obj.real * obj.real + obj.imag * obj.imag);
    result.imag = (imag * obj.real - real * obj.imag) / (obj.real * obj.real + obj.imag * obj.imag);
    return result;
}

complex complex::conj()
{
    complex result;
    result.real = real;
    result.imag = -imag;
    return result;
}

float complex::abs()
{
    return sqrt(real*real + imag*imag);
}

float complex::angle()
{
    return atan2(imag, real);
}


complex exp(const complex &obj)
{
    complex result;
    result.real = pow((double)2.718281828459045, (double)obj.real) * cos(obj.imag);
    result.imag = pow((double)2.718281828459045, (double)obj.real) * sin(obj.imag);
    return result;
}
