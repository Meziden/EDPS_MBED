#include "mbed.h"

//TDPS_CMD
Serial usbport(USBTX,USBRX);

//Your hardware resources here

int scheduler(const char* cmd_tmp,const char* arg_tmp);

int main()
{
    //Initialize
    
    usbport.format(8,SerialBase::None,1);
    usbport.baud(9600);
    
    usbport.printf("Command Interpreter Ready.\r\n");
    
    while(1)
    {
        char cmd_buf[32] = {'\0'};
        char tmp_cmd[32]  = {'\0'};
        char tmp_arg[32]  = {'\0'};
        //read commands
        int arg_flag = 0; 
        usbport.gets(cmd_buf,32);
        
        for(size_t i=0; i!=32; i++)
        {
            if(cmd_buf[i] == '\0')
                break;
            if(cmd_buf[i] == ' ' || cmd_buf[i] == '\n')
                continue;
            
            if(cmd_buf[i] == '-')
                arg_flag = i;
            
            if(cmd_buf[i] <= '9' && cmd_buf[i] >= '0' && (!arg_flag))  // is a number
                arg_flag = i;                                          // also used as offset
            
            if(arg_flag)
                tmp_arg[i - arg_flag] = cmd_buf[i];
            else
                tmp_cmd[i] = cmd_buf[i];
        }
        
        if(scheduler(tmp_cmd,tmp_arg))
            usbport.printf("PASS\r\n");
        else
            usbport.printf("ERROR: UNEXPECTED COMMAND\r\n");
    }
    return 0;
}

int scheduler(const char* cmd_tmp,const char* arg_tmp)
{
    //args and stats.
    int argi_tmp = atoi(arg_tmp);
    float stat_tmp = 0;
    
    if(!strcmp("Command1",cmd_tmp))
    {
        //your operation for this command here
        return 1;
    }
    
    if(!strcmp("Command2",cmd_tmp))
    {
        //your operation for this command here
        return 2;
    }
    
    if(!strcmp("Command3",cmd_tmp))
    {
        //your operation for this command here
        return 3;
    }
    
    if(!strcmp("PING",cmd_tmp))
    {
        return 666;
    }
    
    //return 0 means unexpected commands
    return 0;
}
