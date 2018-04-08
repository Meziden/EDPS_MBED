# mbed-libmezi

### 一些mbed类库
  
**警告: 某些重载构造函数的预设针脚名(PinName)只能用于NXP LPC1768开发板，如果您使用不同的硬件平台，请在创建对象时使用正确的PinName，否则可能导致硬件损坏。** 
  
### max7219 / MAX7219 8x8点阵LED驱动器
- 封装自SPI接口，支持多路显示。
- 包含文件 max7219.h max7219.cpp，示例程序为main_max7219.cpp

### l298n / L298N 两相步进电机驱动器
- 封装自GPIO，支持同时控制多路电机。
- 包含文件 l298n.h l298n.cpp，示例程序为main_l298n.cpp

### xy160d / XY160D 直流电机驱动器
- 封装自PWM，已加入正反转间刹车功能。
- 包含文件 xy160d.h xy160d.cpp，示例程序为tan90°

### psensor / psensor_v2 / 莫合实业 试验型弯曲传感器
- psensor.h包含了输入输出接口
- psensor_v2.h为纯数据对象类，使用psensor::write()更新数据

### amux16 / 16路模拟信号 多路复用器
- 包含文件 amux16.h amux16.cpp

### hmc5883l / HMC5883L IIC接口 三轴磁场计
- 封装自I2C，可手动输入校准值,自动校准tan90°
- 提供原始数据输出接口，可根据该数据算出椭圆/球的偏移及比例。
- 包含文件 hmc5883l.h hmc5883l.cpp，示例程序为main_hmc5883l.cpp
