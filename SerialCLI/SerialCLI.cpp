#include "mbed.h"
#include "SerialCLI.h"

SerialCLI::SerialCLI(PinName pin_tx, PinName pin_rx, int baudrate) : Serial(pin_tx, pin_rx, baudrate)
{
    // Function Table Initialization
    for(size_t i = 0; i != FUNCTION_TABLE_SIZE; i++)
    {
        strcpy(m_function_table[i].node_name, "");
        m_function_table[i].node_func = NULL;
    }
    
    // Overflow Table Initialization
    for(size_t i = 0; i != OVERFLOW_TABLE_SIZE; i++)
    {
        strcpy(m_overflow_table[i].node_name, "");
        m_overflow_table[i].node_func = NULL;
    }
    
    m_overflow_used = 0;
    
    attach(callback(this, &SerialCLI::rxirq_clb), Serial::RxIrq);
    
    if(SCHEDULER_CLI)
        printf("\n[SerialCLI] Initialization Complete.\nserialcli >");
}

void SerialCLI::rxirq_clb()
{
    // Get Command
    char cmd_buf[SCHEDULER_BUFSIZE]  = {'\0'};
    gets(cmd_buf, SCHEDULER_BUFSIZE);
    
    // Command Line Echo
    if(SCHEDULER_CLI)
        printf("%s",cmd_buf);
    
    // Exception Handling
    switch(scheduler(cmd_buf))
    {
        case -1:
            if(SCHEDULER_CLI)
                printf("[SerialCLI] Empty Command.\n");
            break;
        case -2:
            if(SCHEDULER_CLI)
                printf("[SerialCLI] Command not found.\n");
            break;
        default:
            break;
    }
    
    // After Executing Command.
    if(SCHEDULER_CLI)
        printf("serialcli >");
}

int SerialCLI::add_function(char* cmd_name, serialcli_fp_t cmd_fp)
{
    // The corresponding main hash table node is now empty.
    if(m_function_table[simplehash(cmd_name)].node_func == NULL)
    {
        printf("Direct Adding.\n");
        // Setting up name.
        strcpy(m_function_table[simplehash(cmd_name)].node_name, cmd_name);
        // Setting up Function Pointer
        m_function_table[simplehash(cmd_name)].node_func = cmd_fp;
    }
    else if(!strcmp(cmd_name, m_function_table[simplehash(cmd_name)].node_name))
    {
        // Registered Command
        if(SCHEDULER_CLI)
            printf("[Error] Command %s already registered.\n",cmd_name);
        return -1;
    }   
    // Using Public Overflow Table
    else
    {   
        printf("Using Overflow Table.\n");
        // Check if the overflow table is full
        if(m_overflow_used == OVERFLOW_TABLE_SIZE)
        {
            if(SCHEDULER_CLI)
                printf("[Error] Hash/Overflow Table got no space for this command.\n");
            return -1;
        }
        
        // Check if the name already exist in overflow table.
        for(size_t i = 0; i != m_overflow_used; i++)
        {
            if(!strcmp(cmd_name, m_overflow_table[i].node_name))
            {
                if(SCHEDULER_CLI)
                    printf("[Error] Command %s already registered.\n",cmd_name);
                return -1;
            }
        }
        
        // Setting up name.
        strcpy(m_overflow_table[m_overflow_used].node_name, cmd_name);
        // Setting up Function Pointer
        m_overflow_table[m_overflow_used].node_func = cmd_fp;
        m_overflow_used++;
        return 0;
    }
    return -1;
}

int SerialCLI::scheduler(char* cmd_buf)
{
    /* Parsing input string. */
    int cmd_argc = 1;
    /* ARGV is only a pointer set for CMDBUF, not real buffers */
    char* cmd_argv[MAX_ARGUMENT] = {cmd_buf};
    
    int space_flag = 0;
    /* Exit if reach the end of buffer */
    for(size_t i = 0; i != SCHEDULER_BUFSIZE; i++)
    {
        /* Exit if end reached or argument number too large */
        if(cmd_buf[i] == '\0' || cmd_buf[i] == '\n' || cmd_buf[i] == '\r')
        {
            cmd_buf[i] = '\0';
            break;
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
    
    // For Empty Command, which simple hash returns 0
    if(!strcmp(cmd_argv[0],""))
        return -1;
    
    // Node is available and no hash collision
    if(!strcmp(cmd_argv[0], m_function_table[simplehash(cmd_argv[0])].node_name))
    {
        // Execute the binded function
        return (*(m_function_table[simplehash(cmd_argv[0])].node_func))(cmd_argc, cmd_argv);
    }
    // Continue Searching in Overflow Table for hash collision situations
    else
    {
        for(size_t i = 0; i != m_overflow_used; i++)
        {
            // Found in Overflow Table
            if(!strcmp(cmd_argv[0], m_overflow_table[i].node_name))
            {
                return (*(m_overflow_table[i].node_func))(cmd_argc, cmd_argv);
            }
        }
    }
    return -2;
}

int SerialCLI::simplehash(char* const cmd_name)
{
    int sum = 0;   // The return value should be 0 when processing empty strings.
    char* cmd_name_tmp = cmd_name;
    for(; (*cmd_name_tmp); cmd_name_tmp++)
        sum += (uint8_t)cmd_name_tmp[0];
    // Ensure the return address is in proper range.
    return (sum % FUNCTION_TABLE_SIZE);
}
