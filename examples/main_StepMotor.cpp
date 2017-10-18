#include "mbed.h"
#include "rtos.h"
#include "StepMotor.h"

//在使用demux之后,StepMotor构造函数参数可以减至一个。
StepMotor motor1(p5,p6,p7,p8);
StepMotor motor2(p9,p10,p11,p12);
DigitalOut indic1(LED1);
DigitalOut indic2(LED2);

Serial usbport(USBTX,USBRX);

Thread thread1;
Thread thread2;

//用于临时中断电机操作，并可稍后继续当前操作
const int SIGNAL_LAUNCH_1 = 0xAA;
const int SIGNAL_LAUNCH_2 = 0xFF;
const int SIGNAL_INTERRUPT = 0x66;


void motor1_thread()
{
    while(1)
    {
        thread1.signal_wait(SIGNAL_LAUNCH_1);
        //周期20ms，转速每秒90度
        indic1=1;
        motor1.rotate(5);
        indic1=0;
    }
}


void motor2_thread()
{
    while(1)
    {
        thread2.signal_wait(SIGNAL_LAUNCH_2);
        indic2=1;
        motor2.rotate(20);
        indic2=0;
    }
}

// main函数在单独的线程上运行
int main()
{
    usbport.format(8,SerialBase::None,1);
    usbport.baud(9600);
    thread1.start(motor1_thread);
    thread2.start(motor2_thread);
    usbport.printf("USAGE:motornumber steps\n");
    
    while(1)
    {
        char cmd[16]={0};
        char cmd_arg[16]={0};
        
        //可在此处读取uart输入并转换为电机操作
        while(!atoi(cmd_arg))
        {
            usbport.printf("cmd@lpc1768 > \n");
            usbport.scanf("%s%s",cmd,cmd_arg);
        }
        usbport.printf("MOTOR:%s,STEPS=%d\n",cmd,atoi(cmd_arg));
        if(!strcmp(cmd,"motor1"))
        {
            motor1.stepcount = atoi(cmd_arg);
            thread1.signal_set(SIGNAL_LAUNCH_1);
            usbport.printf("task sent to motor1\n");
        }
        if(!strcmp(cmd,"motor2"))
        {
            motor2.stepcount = atoi(cmd_arg);
            thread2.signal_set(SIGNAL_LAUNCH_2);
            usbport.printf("task sent to motor2\n");
        }
    }
    return 0;
}
