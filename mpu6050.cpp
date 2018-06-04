#include "mbed.h"
#include "mpu6050.h"

mpu6050::mpu6050(PinName sda_pin,PinName scl_pin):I2C(sda_pin,scl_pin)
{
    for(size_t i = 0; i != 6; i++)
        buffer[i] = 0;
    // Use 400KHz IIC Speed
    frequency(400000);
    writebyte(REG_POWER1, 0x80);
    wait_ms(100);
    writebyte(REG_POWER1, 0x00);
    
    setrange_gyro(RANGE_GYRO_500DPS);
    setrange_accel(RANGE_ACCEL_2G);
    
    writebyte(REG_FIFO, 0x00);
    writebyte(REG_SMPLRT, 0x00);
    writebyte(REG_CONFIG, 0x06);
    writebyte(REG_POWER2, 0x00);
}

int mpu6050::update()
{
    //get raw data and store in buffer
    getraw(buffer);
    
    //not using radians
    gyroscope[AXIS_X] = (float)buffer[AXIS_X] * (range_gyro / 32768.0) * (PI/360.0);
    gyroscope[AXIS_Y] = (float)buffer[AXIS_Y] * (range_gyro / 32768.0) * (PI/360.0);
    gyroscope[AXIS_Z] = (float)buffer[AXIS_Z] * (range_gyro / 32768.0) * (PI/360.0);
    
    accelerometer[AXIS_X] = (float)buffer[AXIS_X + 3] * (range_accel / 32768.0);
    accelerometer[AXIS_Y] = (float)buffer[AXIS_Y + 3] * (range_accel / 32768.0);
    accelerometer[AXIS_Z] = (float)buffer[AXIS_Z + 3] * (range_accel / 32768.0);
                                
    return 0;
}

void mpu6050::writebyte(uint8_t reg_addr,uint8_t data)
{
    start();
    write(ADDR_WRITE);
    write(reg_addr);
    write(data);
    stop();
}

uint8_t mpu6050::readbyte(uint8_t reg_addr)
{
    start();
    write(ADDR_READ);
    write(reg_addr);
    uint8_t result = read(0);
    stop();
    return result;
}

int mpu6050::getraw(int* bufferptr)
{
    uint8_t gyrobuffer[6] = {0};
    uint8_t accelbuffer[6] = {0};
    
    start();
    write(ADDR_WRITE);
    write(REG_GYROSTORE);
    stop();
    
    start();
    write(ADDR_READ);
    for(size_t i = 0; i != 5; i++)
        gyrobuffer[i] = read(1);
    gyrobuffer[5] = read(0);
    stop();
    
    start();
    write(ADDR_WRITE);
    write(REG_ACCELSTORE);
    stop();
    
    start();
    write(ADDR_READ);
    for(size_t i = 0; i != 5; i++)
        accelbuffer[i] = read(1);
    accelbuffer[5] = read(0);
    stop();
    
    //Data Processing
    bufferptr[AXIS_X] = (int)(int16_t)((gyrobuffer[0] << 8) + gyrobuffer[1]);
    bufferptr[AXIS_Y] = (int)(int16_t)((gyrobuffer[2] << 8) + gyrobuffer[3]);
    bufferptr[AXIS_Z] = (int)(int16_t)((gyrobuffer[4] << 8) + gyrobuffer[5]);
    
    bufferptr[AXIS_X + 3] = (int)(int16_t)((accelbuffer[0] << 8) + accelbuffer[1]);
    bufferptr[AXIS_Y + 3] = (int)(int16_t)((accelbuffer[2] << 8) + accelbuffer[3]);
    bufferptr[AXIS_Z + 3] = (int)(int16_t)((accelbuffer[4] << 8) + accelbuffer[5]);
    
    
    return 0;
}

void mpu6050::setrange_gyro(uint8_t range)
{
    uint8_t reg_gyro_tmp = readbyte(REG_GYRO);
    reg_gyro_tmp &= ~(3 << 3);
    reg_gyro_tmp |= range;
    writebyte(REG_GYRO, reg_gyro_tmp);
    
    switch(range)
    {
        //using degree
        case RANGE_GYRO_250DPS:     range_gyro = 250;break;
        case RANGE_GYRO_500DPS:     range_gyro = 500;break;
        case RANGE_GYRO_1000DPS:    range_gyro = 1000;break;
        case RANGE_GYRO_2000DPS:    range_gyro = 2000;break;
    }
}

void mpu6050::setrange_accel(uint8_t range)
{
    uint8_t reg_accel_tmp = readbyte(REG_ACCEL);
    reg_accel_tmp &= ~(3 << 3);
    reg_accel_tmp |= range;
    writebyte(REG_ACCEL, reg_accel_tmp);
    
    switch(range)
    {
        //g = 9.81m/s^2
        case RANGE_GYRO_250DPS:     range_accel = 2*9.81;break;
        case RANGE_GYRO_500DPS:     range_accel = 4*9.81;break;
        case RANGE_GYRO_1000DPS:    range_accel = 8*9.81;break;
        case RANGE_GYRO_2000DPS:    range_accel = 16*9.81;break;
    }
}
