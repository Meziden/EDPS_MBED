#include "mbed.h"

/* Hardware Objects */ 
Serial serial_port(D1, D0);
//For NXP LPC1768， usethe following instead
//Serial serial_port(USBTX, USBRX);
DigitalOut myled(LED1);

/* Command Interpreter Parameters */
const int MAX_ARGUMENT = 10;
const int SCHEDULER_BUFSIZE = 320;  // 320bytes
const bool SCHEDULER_ECHO = 1;

void cli_callback(void);
int scheduler(int argc, char** argv);

int main()
{
    /* Wake up every buddy. */
    serial_port.baud(115200);
    serial_port.format(8, SerialBase::None, 1);
    serial_port.attach(&cli_callback, Serial::RxIrq);
    
    if(SCHEDULER_ECHO)
        serial_port.printf("Serial @ STM32F303K8 > ");
    
    while(1)
    {
        myled = 1;
        wait(1);
        myled =0;
        wait(1);
    }
}

/*
 * Meziden's Simple Command Interpreter
 * CLI Subsystem v2.0, comes with optimized memory use, multi-argument support and much, much more.
 */

void cli_callback(void)
{
    char cmd_buf[SCHEDULER_BUFSIZE]  = {'\0'};
    
    /* Get Command */
    serial_port.gets(cmd_buf,SCHEDULER_BUFSIZE);
    
    if(SCHEDULER_ECHO)
        serial_port.printf("%s",cmd_buf);
    
    /* Parsing input string. */
    int cmd_argc = 1;
    char* cmd_argv[MAX_ARGUMENT] = {cmd_buf};
    int space_flag = 0;
    /* Exit if reach the end of buffer */
    for(size_t i = 0; i != SCHEDULER_BUFSIZE; i++)
    {
        /* Exit if end reached or argument number too large */
        if(cmd_buf[i] == '\0' || cmd_buf[i] == '\n' || cmd_buf[i] == '\r')
        {
            cmd_buf[i] = '\0';
        }
        
        if(cmd_argc == MAX_ARGUMENT)
            break;
        
        if(cmd_buf[i] == ' ')
        {
            space_flag = 1;
            cmd_buf[i] = '\0';
            continue;
        }

        if(space_flag)
        {
            space_flag = 0;
            cmd_argv[cmd_argc] = cmd_buf + i;
            cmd_argc++;
        }
    }
    
    /* Exception Handling */
    if(scheduler(cmd_argc,cmd_argv) < 0)
        serial_port.printf("[CLI] error:unexpected command\n");
    else
        serial_port.printf("[CLI] task complete.\n");
    
    if(SCHEDULER_ECHO)
        serial_port.printf("Serial @ STM32F303K8 > ");
}

int scheduler(int argc, char** argv)
{
    //args and stats.
    if(!strcmp(argv[0],"hello"))
    {
        serial_port.printf("Hello!\n");
        return 1;
    }
    return -1;
}
