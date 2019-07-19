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
    char cmd_buf[SCHEDULER_BUFSIZE]  = {'\0'};
    
    /* Get Command */
    gets(cmd_buf,SCHEDULER_BUFSIZE);
    
    if(SCHEDULER_CLI)
        printf("%s",cmd_buf);
    
    scheduler(cmd_buf);
    
    // After Executing Command.
    if(SCHEDULER_CLI)
        printf("serialcli >",cmd_buf);
    
    //TODO: Exception Handling
}

int SerialCLI::add_function(char* cmd_name, serialcli_fp_t cmd_fp)
{
    // The corresponding main hash table node is now empty.
    if(m_function_table[simplehash(cmd_name)].node_func == NULL)
    {
        // Setting up name.
        strcpy(m_function_table[simplehash(cmd_name)].node_name, cmd_name);
        // Setting up Function Pointer
        m_function_table[simplehash(cmd_name)].node_func = cmd_fp;
    }
    // Using Public Overflow Table
    else
    {
        // Check if the name already exist.
        if(!strcmp(cmd_name, m_function_table[simplehash(cmd_name)].node_name))
        {
            if(SCHEDULER_CLI)
                printf("[Error] Command %s already registered.\n",cmd_name);
            return -1;
        }
        
        // Check if the overflow table is full
        if(m_overflow_used == OVERFLOW_TABLE_SIZE)
        {
            if(SCHEDULER_CLI)
                printf("[Error] Hash/Overflow Table got no space for this command.\n");
            return -1;
        }
        
        // Setting up name.
        strcpy(m_overflow_table[m_overflow_used].node_name, cmd_name);
        // Setting up Function Pointer
        m_overflow_table[m_overflow_used].node_func = cmd_fp;
        m_overflow_used++;
    }
    return 0;    
}

int SerialCLI::scheduler(char* cmd_buf)
{
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
    
    int result = 0;
    
    // Node is available and no hash collision
    if(!strcmp(cmd_argv[0], m_function_table[simplehash(cmd_argv[0])].node_name))
    {
        // Execute the binded function
        result = (*(m_function_table[simplehash(cmd_argv[0])].node_func))(cmd_argc, cmd_argv);
    }
    // Continue Searching in Overflow Table for hash collision
    else
    {
        for(size_t i = 0; i != m_overflow_used; i++)
        {
            // Reached end of the overflow table
            if(!strcmp("", m_function_table[i].node_name))
                break;
            // Found in Overflow Table
            if(!strcmp(cmd_argv[0], m_function_table[i].node_name))
                result = (*(m_function_table[simplehash(cmd_argv[0])].node_func))(cmd_argc, cmd_argv);
        }
        // Not found in whole Hash Table
        printf("[Error] Command %s not found.\n", cmd_argv[0]);
        result = -1;
    }
    
    return result;
}

int SerialCLI::simplehash(char* cmd_name)
{
    int sum;
    for(sum = 0; cmd_name[0] != '\0'; cmd_name++)
        sum += (int)cmd_name[0];
    // Ensure the return address is in proper range.
    return (sum % FUNCTION_TABLE_SIZE);
}
