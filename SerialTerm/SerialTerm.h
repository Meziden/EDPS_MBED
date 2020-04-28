/* 
 * SerialTerm: CLI Command Interpreter for Serial Ports
 * 
 * CHANGELOG v3.1:
 * + Enhanced ANSI Terminal Support powered by Simon Ford
 * + History Support
 */

#ifndef SERIALTERM_H
#define SERIALTERM_H

/* These configurations need to be adjusted when problems shows up in memory allocation. */

// Function Name Length Limit
const int FUNCTION_NAME_LIMIT = 16;

// Using Public Overflow Table to solve hash collision
const int FUNCTION_TABLE_SIZE = 32;
const int OVERFLOW_TABLE_SIZE = 16;

const int HISTORY_MAX_LIMIT = 16;

// Command Interpreter Parameters
const int CMD_MAX_ARGUMENT = 16;
const int CMD_LINE_BUFSIZE =128;  // 64bytes
const bool ENABLE_INTERACTIVE = 1;      // Disable when acting as duplex RPC framework.
const bool ENABLE_COLOR = 1;      // Disable when using common serial monitors

// SerialTerm Function Typedef
typedef int (*serialcli_fp_t)(int, char**);

// SerialTerm Function Node Typedef
typedef struct
{
    char node_name[FUNCTION_NAME_LIMIT];
    serialcli_fp_t node_func;
}
serialcli_node_t;

// Class Definition
class SerialTerm : public RawSerial
{
    public:
        // Constructor, setting up pins and baudrate
        SerialTerm(PinName pin_tx, PinName pin_rx, int baudrate = 115200);
        
        // Add Functions
        int add_function(const char* cmd_name, serialcli_fp_t cmd_fp);
        
        // Display Layout
        int display_functions(void);
        
        // Command Interpreter Function
        int scheduler(char* cmd_str);
        
        // ANSI Terminal Support
        void greetings();
        void csi_sgr(int arg);
        void color_front(int color);
        void color_back(int color);
        void cursor_locate(int col, int row);
        void clear();
        
    private:
        // USB RX Callback, automatically called when commands coming in.
        void rxirq_clb();
        
        // Function - Name Bidings, use hash() + strcmp() for higher efficiency.
        int simplehash(const char* const cmd_name);
        
        // Function Pointer Table
        serialcli_node_t m_function_table[FUNCTION_TABLE_SIZE];
        
        // Solving Hash Collision
        serialcli_node_t m_overflow_table[OVERFLOW_TABLE_SIZE];
        int m_overflow_used;
        
        // Command Line Buffer
        char m_cmdline_buf[CMD_LINE_BUFSIZE];
        int m_cmdline_used;
        
        // Command History Buffer
        char m_history_buf[HISTORY_MAX_LIMIT][CMD_LINE_BUFSIZE];
        char* m_history_ptr[HISTORY_MAX_LIMIT];
        int m_history_cursor;
        
        // Indicator
        DigitalOut m_led;
};

#endif
