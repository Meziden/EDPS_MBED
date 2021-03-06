#ifndef SPECTRUM_H
#define SPECTRUM_H

/*
 * Data Structure for Spectrum UI v1.1
 * with static members, no need for global variables
 */

const int SUBNODE_MAX = 6;
const int NODE_TYPE_SUBFNC = 0;
const int NODE_TYPE_DAEMON = 1;
const float DAEMON_PERIOD = 0.02;

// Single SpectrumGL Node Definition
class spectrum
{
    public:
    spectrum(const char* node_name, void (*node_func)(void* arg) = NULL, void (*prev_func)(void* arg) = NULL, const int node_type = NODE_TYPE_SUBFNC);
    
    // Node Operations
    int add_node(spectrum* node_ptr);
    
    // Parent Node
    spectrum* m_parent;
    static spectrum* s_nodenow;
    static int s_rootflag;
    static int s_cursor;
    
    // Node Name, Node Type and Function
    char m_name[16];
    int m_type;
    void (*m_func)(void* arg);
    
    // if run as a Daemon.
    Ticker m_ticker;
    
    // Data Preview Interface
    void (*m_prev)(void* arg);
    
    // Subnode
    spectrum* subnode_vec[SUBNODE_MAX];
    int subnode_num;
};

#endif
