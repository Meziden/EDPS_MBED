//A simple command interpreter, only supports one command and one argument for each line.
//It may support more arguments by using malloc, probably.
#include "mbed.h"

//Serial Port
Serial usbport(USBTX,USBRX);

int scheduler(const char* cmd_tmp,const char* arg_tmp);

int main()
{
    usbport.format(8,SerialBase::None,1);
    usbport.baud(9600);
    usbport.printf("\nStarting USB-UART port.\n");
    
    // Command Interpreter
    while(1)
    {
        char cmd_buf[32] = {'\0'};
        char tmp_cmd[32]  = {'\0'};
        char tmp_arg[32]  = {'\0'};
        //read commands
        int arg_flag = 0;
        usbport.printf("usbuart @ lpc1768 > ");
        usbport.gets(cmd_buf,32);
        usbport.printf("%s",cmd_buf);
        
        //parsing input string
        for(size_t i=0; i!=32; i++)
        {
            if(cmd_buf[i] == '\0')
                break;
            if(cmd_buf[i] == ' ' || cmd_buf[i] == '\n')
                continue;
            
            if(cmd_buf[i] <= '9' && cmd_buf[i] >= '0' && (!arg_flag))  // is a number
                arg_flag = i;                                          // also used as offset
            
            if(cmd_buf[i] == '-')
                arg_flag = i;
            
            if(arg_flag)
                tmp_arg[i - arg_flag] = cmd_buf[i];
            else
                tmp_cmd[i] = cmd_buf[i];
        }
        
        if(scheduler(tmp_cmd,tmp_arg))
            usbport.printf("[Interpreter] task complete.\n");
        else
            usbport.printf("[Interpreter] error:unexpected command\n");
    }
    return 0;
}

int scheduler(const char* cmd_tmp,const char* arg_tmp)
{
    //args and stats.
    int argi_tmp = atoi(arg_tmp);
    
    //Add more if statements to support YOUR commands.
    if(!strcmp("help",cmd_tmp))
    {
        usbport.printf("supported commands are:\nhelp version\n");
        return 1;
    }
    
    if(!strcmp("version",cmd_tmp))
    {
        usbport.printf("Simple CLI for ARM mbed v1.0\nBy the naive, for the naive.\n");
        return 1;
    }
    
    return 0;
}
