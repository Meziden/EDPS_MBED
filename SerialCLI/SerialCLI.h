/* 
 * SerialCLI: CLI Command Interpreter for Serial Ports
 * Designed for Exoskeleton Control System 2.1
 * 
 * CHANGELOG from v2.0:
 * + OOP API.
 * + Faster and More General Command Parsing.
 * + Command Function Hashtable: Faster Function Match.
 */

#ifndef SERIALCLI_H
#define SERIALCLI_H

// Function Name Length Limit
const int FUNCTION_NAME_LIMIT = 16;

// Using Public Overflow Table to solve hash collision
const int FUNCTION_TABLE_SIZE = 32;
const int OVERFLOW_TABLE_SIZE = 16;

// Command Interpreter Parameters
const int MAX_ARGUMENT = 10;
const int SCHEDULER_BUFSIZE = 320;  // 320bytes
const bool SCHEDULER_CLI = 1;       // Disable when using two SerialCLI for communication.

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
class SerialCLI : public Serial
{
    public:
        // Constructor, setting up pins and baudrate
        SerialCLI(PinName pin_tx, PinName pin_rx, int baudrate = 115200);
        
        // Add Functions
        int add_function(char* cmd_name, serialcli_fp_t cmd_fp);
        
        // Command Interpreter Function
        int scheduler(char* cmd_str);
        
    private:
        // USB RX Callback, automatically called when commands coming in.
        void rxirq_clb();
        
        // Function - Name Bidings, use hash() + strcmp() for higher efficiency.
        int simplehash(char* cmd_name);
        
        // Function Pointer Table
        serialcli_node_t m_function_table[FUNCTION_TABLE_SIZE];
        
        // Solving Hash Collision
        serialcli_node_t m_overflow_table[OVERFLOW_TABLE_SIZE];
        int m_overflow_used;
};

#endif
