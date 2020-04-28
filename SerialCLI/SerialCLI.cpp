#include "mbed.h"
#include "SerialCLI.h"

SerialCLI::SerialCLI(PinName pin_tx, PinName pin_rx, int baudrate) : RawSerial(pin_tx, pin_rx, baudrate), m_led(LED2)
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
    
    // Command Line Buffer
    for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
        m_cmd_buf[i] = '\0';
    m_buf_used = 0;
    
    attach(callback(this, &SerialCLI::rxirq_clb), Serial::RxIrq);
    
    if(ENABLE_INTERACTIVE)
    {
        puts("SerialTerm v3.1 on ST NUCLEO-H743ZI\r\n--------------------\r\n");
        puts("Configuration:\r\n");
        
        printf("  - Buffer Length: %d\r\n  - Max Argument: %d\r\n", CMD_LINE_BUFSIZE, CMD_MAX_ARGUMENT);
        printf("  - Hash Table: %d\r\n  - Overflow Table: %d\r\n", FUNCTION_TABLE_SIZE, OVERFLOW_TABLE_SIZE);

        puts("Memory Usage:\r\n");

        printf("  - Function Table: %d Bytes\r\n",(OVERFLOW_TABLE_SIZE + FUNCTION_TABLE_SIZE) * sizeof(serialcli_node_t));
        printf("  - Line Buffer: %d + %d Bytes\r\n", CMD_LINE_BUFSIZE, CMD_MAX_ARGUMENT * 4);
        puts("--------------------\r\n");
        printf("cli@stm32h743zi $ ");
    }
}

void SerialCLI::rxirq_clb()
{
    m_led = 1;
    // Get Command
    while(readable())
    {
        m_cmd_buf[m_buf_used] = getc();
        if(ENABLE_INTERACTIVE)
        {
            if(m_cmd_buf[m_buf_used] == 3)   // STX: Start of Text
                continue;
            if(m_cmd_buf[m_buf_used] == 4)   // ETX: End of Text
            {   
                break;
            }
            if(m_cmd_buf[m_buf_used] == 8 || m_buf_used == (CMD_LINE_BUFSIZE - 1)) // BS: Backspace
            {
                if(m_buf_used != 0)
                {
                    m_buf_used--;
                    m_cmd_buf[m_buf_used] = '\0';
                    putc('\b');
                    putc(' ');
                    putc('\b');
                }
            }
            else if(m_cmd_buf[m_buf_used] == 27)     // Cursor Operations([[A, [[B, [[C, [[D)
            {
                getc(); // Dummy byte
                getc(); // A,B,C,D Ignored.
            }
            else
            {
                putc(m_cmd_buf[m_buf_used]);
                m_buf_used++;
            }
        }
        else
        {
            if(m_buf_used == (CMD_LINE_BUFSIZE - 1)) // Full
            {
                m_buf_used--;
                m_cmd_buf[m_buf_used] = '\0';
            }
            else
                m_buf_used++;
        }
        
        if((m_buf_used != 0) && (m_cmd_buf[m_buf_used - 1] == '\r' | m_cmd_buf[m_buf_used - 1] == '\n'))
        {
            if(ENABLE_INTERACTIVE)
                printf("\r\n");
            // Command Parsing and Execution 
            switch(scheduler(m_cmd_buf))
            {
                case -1:
                    break;
                case -2:
                    if(ENABLE_INTERACTIVE)
                        printf("Error: %s not found.\r\n", m_cmd_buf);
                    break;
                default:
                    break;
            }
            
            // Reset Command Line Buffer
            for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
                m_cmd_buf[i] = '\0';
            m_buf_used = 0;
            
            // if(RPC_ACK)
            // putc(0x04);
            
            // After Executing Command.
            if(ENABLE_INTERACTIVE)
                printf("cli@stm32h743zi $ ");
        }   
    }
    m_led = 0;
}

int SerialCLI::add_function(const char* cmd_name, serialcli_fp_t cmd_fp)
{
    if(cmd_fp == NULL || cmd_name[0] == '\0' )
    {
        if(ENABLE_INTERACTIVE)
            printf("Error: The command or function is empty.\r\n");
        return 0;
    }
    
    // The corresponding main hash table node is now empty.
    if(m_function_table[simplehash(cmd_name)].node_func == NULL)
    {
        // Setting up name.
        strcpy(m_function_table[simplehash(cmd_name)].node_name, cmd_name);
        // Setting up Function Pointer
        m_function_table[simplehash(cmd_name)].node_func = cmd_fp;
    }
    else if(!strcmp(cmd_name, m_function_table[simplehash(cmd_name)].node_name))
    {
        // Registered Command
        if(ENABLE_INTERACTIVE)
            printf("Error: %s already registered.\r\n",cmd_name);
        return -1;
    }   
    // Using Public Overflow Table
    else
    {   
        // Check if the overflow table is full
        if(m_overflow_used == OVERFLOW_TABLE_SIZE)
        {
            if(ENABLE_INTERACTIVE)
                printf("Error: Hash/Overflow Table got no space for this command.\r\n");
            return -1;
        }
        
        // Check if the name already exist in overflow table.
        for(size_t i = 0; i != m_overflow_used; i++)
        {
            if(!strcmp(cmd_name, m_overflow_table[i].node_name))
            {
                if(ENABLE_INTERACTIVE)
                    printf("Error: Command %s already registered.\r\n", cmd_name);
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
    char* cmd_argv[CMD_MAX_ARGUMENT] = {cmd_buf};
    
    int space_flag = 0;
    /* Exit if reach the end of buffer */
    for(size_t i = 0; i != CMD_LINE_BUFSIZE; i++)
    {
        /* Exit if end reached or argument number too large */
        if((cmd_buf[i] == '\0') | (cmd_buf[i] == '\n') | (cmd_buf[i] == '\r'))
        {
            cmd_buf[i] = '\0';
            break;
        }
        
        if(cmd_argc == CMD_MAX_ARGUMENT)
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

int SerialCLI::simplehash(const char* cmd_name)
{
    int sum = 0;   // The return value should be 0 when processing empty strings.
    for(; (*cmd_name); cmd_name++)
        sum += (uint8_t)cmd_name[0];
    // Ensure the return address is in proper range.
    return (sum % FUNCTION_TABLE_SIZE);
}

int SerialCLI::display_functions(void)
{
    puts("SerialCLI - Memory Layout\r\n");
    // Main Hash Table
    puts("[Main Hash Table]\r\n----------------\r\n");
    size_t count = 0;
    for(size_t i = 0; i != FUNCTION_TABLE_SIZE; i++)
    {
        if(m_function_table[i].node_func != NULL)
        {
            printf("[%2d] %-12s 0x%X\r\n", i,m_function_table[i].node_name, (uint32_t)m_function_table[i].node_func);
            count++;
        }
    }
    printf("Usage: %d / %d\r\n", count, FUNCTION_TABLE_SIZE);
    
    // Overflow Table
    puts("\r\n[Overflow Table]\r\n------------------\r\n");
    for(size_t i = 0; i != m_overflow_used; i++)
    {
        printf("[%2d] %-12s 0x%X\r\n", i, m_overflow_table[i].node_name, (uint32_t)m_overflow_table[i].node_func);
    }
    printf("Usage: %d / %d\r\n", m_overflow_used, OVERFLOW_TABLE_SIZE);
    puts("\r\n[End of Table]\r\n");
    
    return 0;
}
