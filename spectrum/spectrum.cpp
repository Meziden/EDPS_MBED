#include "mbed.h"
#include "spectrum.h"

spectrum::spectrum(const char* node_name, void (*node_func)(void))
{
    strcpy(m_name,node_name);
    m_func = node_func;
    
    for(size_t i = 0; i!=5; i++)
        subnode_vec[i] = NULL;
    
    subnode_num = 0;
    m_parent = NULL;
}

int spectrum::add_node(spectrum* node_ptr)
{
    if(subnode_num != 5)
    {
        node_ptr->m_parent = this;
        subnode_vec[subnode_num] = node_ptr;
        subnode_num++;
        return 0;
    }
    else
        return -1;
}
