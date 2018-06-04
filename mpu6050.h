// WARNING: this is not a complete encapsulation for MPU6050, functions like external IIC for magnetometer is disabled.
#ifndef MPU6050_H
#define MPU6050_H

// parameters for 6 DoF sensor fusion calculations
const float PI = 3.14159265358979323846f;

//AD0 to VCC, thanks honey. 
const uint8_t ADDR_READ  = (0x69 << 1) + 1;
const uint8_t ADDR_WRITE = (0x69 << 1);

//Configuation Storage Registers
const uint8_t REG_POWER1 = 0x6B;
const uint8_t REG_GYRO  = 0x1B;
const uint8_t REG_ACCEL = 0x1C;
const uint8_t REG_FIFO  = 0x23;
const uint8_t REG_SMPLRT = 0x19;
const uint8_t REG_CONFIG  = 0x1A;
const uint8_t REG_POWER2 = 0x6C;

//Gyro / Accelero Range Flags
const uint8_t RANGE_GYRO_250DPS  = 0x00 << 3;
const uint8_t RANGE_GYRO_500DPS  = 0x01 << 3;   //Default Configuarion
const uint8_t RANGE_GYRO_1000DPS = 0x02 << 3;
const uint8_t RANGE_GYRO_2000DPS = 0x03 << 3;

const uint8_t RANGE_ACCEL_2G  = 0x00 << 3;    //Default Configuarion
const uint8_t RANGE_ACCEL_4G  = 0x01 << 3;
const uint8_t RANGE_ACCEL_8G  = 0x02 << 3;
const uint8_t RANGE_ACCEL_16G = 0x03 << 3;

//Data Storage Register
const uint8_t REG_GYROSTORE = 0x43;
const uint8_t REG_ACCELSTORE = 0x3B;

const int AXIS_X = 0;
const int AXIS_Y = 1;
const int AXIS_Z = 2;

class mpu6050 : public I2C
{
    public:
    //Constructor
    mpu6050(PinName sda_pin, PinName scl_pin);
    
    //measurements
    int update();
    //get-raw-data function
    int getraw(int* bufferptr);
    
    //configs
    void setrange_gyro(uint8_t range);
    void setrange_accel(uint8_t range);
    
    //readable components
    float gyroscope[3];
    float accelerometer[3];
    
    private:
    //read-write utilities
    void writebyte(uint8_t reg_addr,uint8_t data);
    uint8_t readbyte(uint8_t reg_addr);
    
    int buffer[6];
    //store the maximum value, or the GGX
    float range_gyro;
    float range_accel;
};

#endif
