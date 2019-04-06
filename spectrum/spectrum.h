#ifndef SPECTRUM_H
#define SPECTRUM_H

// Single SpectrumGL Node Definition
class spectrum
{
    public:
    spectrum(const char* node_name, void (*node_func)(void));
    
    // Node Operations
    int add_node(spectrum* node_ptr);
    
    // Parent Node
    spectrum* m_parent;
    
    spectrum* subnode_vec[5];
    int subnode_num;
    
    char m_name[16];
    void (*m_func)(void);
};

#endif
