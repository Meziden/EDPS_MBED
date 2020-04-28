/* 
 * SerialCLI: CLI Command Interpreter for Serial Ports
 * 
 * CHANGELOG v3.1:
 * + ANSI/VT102 Interactive Terminal Support
 * + Performance Optimization
 */

#ifndef SERIALCLI_H
#define SERIALCLI_H

/* These configurations need to be adjusted when problems shows up in memory allocation. */

// Function Name Length Limit
const int FUNCTION_NAME_LIMIT = 16;

// Using Public Overflow Table to solve hash collision
const int FUNCTION_TABLE_SIZE = 32;
const int OVERFLOW_TABLE_SIZE = 16;

// Command Interpreter Parameters
const int CMD_MAX_ARGUMENT = 10;
const int CMD_LINE_BUFSIZE = 64;  // 64bytes
const bool ENABLE_INTERACTIVE = 0;      // Disable when acting as duplex RPC framework.

// SerialCLI Function Typedef
typedef int (*serialcli_fp_t)(int, char**);

// SerialCLI Function Node Typedef
typedef struct
{
    char node_name[FUNCTION_NAME_LIMIT];
    serialcli_fp_t node_func;
}
serialcli_node_t;

// Class Definition
class SerialCLI : public RawSerial
{
    public:
        // Constructor, setting up pins and baudrate
        SerialCLI(PinName pin_tx, PinName pin_rx, int baudrate = 115200);
        
        // Add Functions
        int add_function(const char* cmd_name, serialcli_fp_t cmd_fp);
        
        // Display Layout
        int display_functions(void);
        
        // Command Interpreter Function
        int scheduler(char* cmd_str);
        
    private:
        // USB RX Callback, automatically called when commands coming in.
        void rxirq_clb();
        
        // Function - Name Bidings, use hash() + strcmp() for higher efficiency.
        int simplehash(const char* cmd_name);
        
        // Function Pointer Table
        serialcli_node_t m_function_table[FUNCTION_TABLE_SIZE];
        
        // Solving Hash Collision
        serialcli_node_t m_overflow_table[OVERFLOW_TABLE_SIZE];
        int m_overflow_used;
        
        // Command Line Buffer
        char m_cmd_buf[CMD_LINE_BUFSIZE];
        int m_buf_used;
        
        // Indicator
        DigitalOut m_led;
};

#endif
