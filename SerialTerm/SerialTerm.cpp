#include "mbed.h"
#include "SerialTerm.h"

SerialTerm::SerialTerm(PinName pin_tx, PinName pin_rx, int baudrate) : RawSerial(pin_tx, pin_rx, baudrate), m_led(LED2)
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
        m_cmdline_buf[i] = '\0';
    m_cmdline_used = 0;
    
    // Command History Buffer
    for(size_t i = 0; i != HISTORY_MAX_LIMIT; i++)
        for(size_t j = 0; j!= CMD_LINE_BUFSIZE; j++)
            m_history_buf[i][j] = '\0';
    
    for(size_t i = 0; i != HISTORY_MAX_LIMIT; i++)
        m_history_ptr[i] = &(m_history_buf[i][0]);
    
    m_history_cursor = -1;
    
    // Configurations and Memory Usage
    if(ENABLE_INTERACTIVE)
    {
        clear();      // Screen Clear
        
        if(ENABLE_COLOR)
            csi_sgr(1);
        puts("SerialTerm v3.1 on ST NUCLEO-H743ZI\r\n--------------------\r\n");
        puts("Configuration:\r\n");
        if(ENABLE_COLOR)
            csi_sgr(0);
        
        printf("  - Buffer Length: %d\r\n  - Max Argument: %d\r\n", CMD_LINE_BUFSIZE, CMD_MAX_ARGUMENT);
        printf("  - Hash Table: %d\r\n  - Overflow Table: %d\r\n", FUNCTION_TABLE_SIZE, OVERFLOW_TABLE_SIZE);
        
        if(ENABLE_COLOR)
            csi_sgr(1);
        puts("Memory Usage:\r\n");
        if(ENABLE_COLOR)
            csi_sgr(0);
        
        printf("  - Function Table: %d Bytes\r\n",(OVERFLOW_TABLE_SIZE + FUNCTION_TABLE_SIZE) * sizeof(serialcli_node_t));
        printf("  - Line Buffer: %d + %d Bytes\r\n", CMD_LINE_BUFSIZE, CMD_MAX_ARGUMENT * 4);
        printf("  - History: %d Bytes\r\n", CMD_LINE_BUFSIZE * HISTORY_MAX_LIMIT);
        puts("--------------------\r\n");
        
        greetings();
    }
    
    attach(callback(this, &SerialTerm::rxirq_clb), SerialBase::RxIrq);
}

void SerialTerm::rxirq_clb()
{
    m_led = 1;
    // Get Command
    while(readable())
    {
        char received_byte = getc();
        if(received_byte == 3)   // STX: Start of Text
            continue;
        if(received_byte == 4)   // ETX: End of Text
        {   
            /* TODO: Undefined Task */
            break;
        }
        if(received_byte == 8 || m_cmdline_used == (CMD_LINE_BUFSIZE - 1)) // BS: Backspace
        {
            if(m_cmdline_used != 0)
            {
                m_cmdline_buf[m_cmdline_used] = '\0';
                m_cmdline_used--;
                putc('\b');
                putc(' ');
                putc('\b');
            }
        }
        else if(received_byte == 27)     // Cursor Operations([[A, [[B, [[C, [[D)
        {
            getc(); // Dummy byte
            received_byte = getc();
            /* History Operations */
            switch(received_byte)
            {
                case 'A':
                    // Not at the top && history is not an empty string
                    if((m_history_cursor != HISTORY_MAX_LIMIT - 1))
                    {
                        m_history_cursor++;
                        
                        if(m_history_ptr[m_history_cursor][0] == '\0')
                        {
                            m_history_cursor--;
                            break;
                        }
                        
                        // Synchronize preview on screen
                        for(size_t i = 0; i != m_cmdline_used; i++)
                            putc('\b');
                        for(size_t i = 0; i != m_cmdline_used; i++)
                            putc(' ');
                        for(size_t i = 0; i != m_cmdline_used; i++)
                            putc('\b');
                        puts(m_history_ptr[m_history_cursor]);
                        
                        // Use History - Reset Command Line Buffer
                        for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
                            m_cmdline_buf[i] = '\0';
                        strncpy(m_cmdline_buf, m_history_ptr[m_history_cursor], strlen(m_history_ptr[m_history_cursor]));
                        m_cmdline_used = strlen(m_cmdline_buf);
                    }
                    break;
                case 'B':
                    // Not at the top && history is not an empty string
                    if(m_history_cursor != -1)
                    {
                        m_history_cursor--;
                        
                        if(m_history_cursor == -1)  // Exit History Mode
                        {
                            // Synchronize preview on screen
                            for(size_t i = 0; i != m_cmdline_used; i++)
                                putc('\b');
                            for(size_t i = 0; i != m_cmdline_used; i++)
                                putc(' ');
                            for(size_t i = 0; i != m_cmdline_used; i++)
                                putc('\b');
                            
                            // Reset Command Line Buffer
                            for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
                                m_cmdline_buf[i] = '\0';
                            m_cmdline_used = strlen(m_cmdline_buf);
                        }
                        else
                        {
                            // Synchronize preview on screen
                            for(size_t i = 0; i != m_cmdline_used; i++)
                                putc('\b');
                            for(size_t i = 0; i != m_cmdline_used; i++)
                                putc(' ');
                            for(size_t i = 0; i != m_cmdline_used; i++)
                                putc('\b');
                            puts(m_history_ptr[m_history_cursor]);
                            
                            // Use History
                            // Reset Command Line Buffer
                            for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
                                m_cmdline_buf[i] = '\0';
                            strncpy(m_cmdline_buf, m_history_ptr[m_history_cursor], strlen(m_history_ptr[m_history_cursor]));
                            m_cmdline_used = strlen(m_cmdline_buf);
                        }
                    }
                    break;
                
                // TODO Left and Right Arrows
                case 'C':
                    break;
                case 'D':
                    break;
            }
        }
        else
        {
            // Write to Command Line Buffer
            m_cmdline_buf[m_cmdline_used] = received_byte;
            m_cmdline_used++;

            // Display on Terminal
            putc(received_byte);

            // Reset History Cursor 
            m_history_cursor = -1;
        }
        
        // if CR / LF detected
        if((m_cmdline_used != 0) && (m_cmdline_buf[m_cmdline_used - 1] == '\r' | m_cmdline_buf[m_cmdline_used - 1] == '\n'))
        {
            // Switch to new line
            printf("\r\n");
            
            // Clear CR / LF
            for(size_t i = 0; i != CMD_LINE_BUFSIZE; i++)
            {
                if((m_cmdline_buf[i] == '\n') | (m_cmdline_buf[i] == '\r'))
                {
                    m_cmdline_buf[i] = '\0';
                    break;
                }
            }
            
            /* Update Used */
            m_cmdline_used = strlen(m_cmdline_buf);
            
            /* Update History */
            if(m_cmdline_buf[0] != '\0')
            {
                // Step 1: Round Shift Buffer Pointers
                char* m_his_tmp = m_history_ptr[HISTORY_MAX_LIMIT - 1];
                for(size_t i = HISTORY_MAX_LIMIT - 1; i != 0; i--)
                    m_history_ptr[i] = m_history_ptr[i - 1];
                m_history_ptr[0] = m_his_tmp;
                
                // Step 2: Fill the history buffer with latest command buffer.
                for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
                    m_history_ptr[0][i] = '\0';
                strncpy(m_history_ptr[0], m_cmdline_buf, strlen(m_cmdline_buf));
                
                // Execute Command
                switch(scheduler(m_cmdline_buf))
                {
                    case -1:
                        break;
                    case -2:
                        if(ENABLE_INTERACTIVE)
                            printf("Error: %s not found.\r\n", m_cmdline_buf);
                        break;
                    default:
                        break;
                }
            }
            
            // Reset Command Line Buffer
            for(size_t i = 0; i!= CMD_LINE_BUFSIZE; i++)
                m_cmdline_buf[i] = '\0';
            m_cmdline_used = 0;
            
            // if(RPC_ACK)
            // putc(0x04);
            
            // After Executing Command.
            if(ENABLE_INTERACTIVE)
                greetings();
        }   
    }
    m_led = 0;
}

int SerialTerm::add_function(const char* cmd_name, serialcli_fp_t cmd_fp)
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

int SerialTerm::scheduler(char* cmd_buf)
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
        if(cmd_buf[i] == '\0' || cmd_argc == CMD_MAX_ARGUMENT)
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

int SerialTerm::simplehash(const char* const cmd_name)
{
    int sum = 0;   // The return value should be 0 when processing empty strings.
    const char* cmd_name_tmp = cmd_name;
    for(; (*cmd_name_tmp); cmd_name_tmp++)
        sum += (uint8_t)cmd_name_tmp[0];
    // Ensure the return address is in proper range.
    return (sum % FUNCTION_TABLE_SIZE);
}

int SerialTerm::display_functions(void)
{
    if(ENABLE_COLOR)
            csi_sgr(1);
    puts("SerialTerm - Memory Layout\r\n");
    
    // Main Hash Table
    puts("[Main Hash Table]\r\n----------------\r\n");
    if(ENABLE_COLOR)
            csi_sgr(0);
    
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
    if(ENABLE_COLOR)
            csi_sgr(1);
    puts("\r\n[Overflow Table]\r\n------------------\r\n");
    if(ENABLE_COLOR)
            csi_sgr(0);
    
    for(size_t i = 0; i != m_overflow_used; i++)
    {
        printf("[%2d] %-12s 0x%X\r\n", i, m_overflow_table[i].node_name, (uint32_t)m_overflow_table[i].node_func);
    }
    printf("Usage: %d / %d\r\n", m_overflow_used, OVERFLOW_TABLE_SIZE);
    puts("\r\n[End of Table]\r\n");
    
    return 0;
}

static int rgb888tobgr111(int colour)
{
    int r = (colour >> 23) & 1;
    int g = (colour >> 15) & 1;
    int b = (colour >> 7) & 1;
    return (b << 2) | (g << 1) | (r << 0);
}

void SerialTerm::greetings()
{
    if(ENABLE_COLOR)
        this->printf("\033[32m\033[1mcli@stm32h743zi\033[0m $ ");
    else
        this->printf("cli@stm32h743zi $ ");
}

void SerialTerm::color_front(int arg)
{
    // Set Attribute Mode    <ESC>[{n}m
    // Foreground Colours : 30 + bgr
    int c = 30 + rgb888tobgr111(arg);
    this->printf("\033[%dm", c);
}

void SerialTerm::color_back(int color)
{
    // Set Attribute Mode    <ESC>[{n}m
    // Background Colours : 40 + bgr
    int c = 40 + rgb888tobgr111(color);
    this->printf("\033[%dm", c);
}

void SerialTerm::csi_sgr(int arg)
{
    this->printf("\033[%dm", arg);
}


void SerialTerm::clear()
{
    this->printf("\033[2J");
}

void SerialTerm::cursor_locate(int col, int row) {
    // Cursor Home    <ESC>[{ROW};{COLUMN}H
    this->printf("\033[%d;%dH", row + 1, col + 1);
}
