#include "mbed.h"
#include "spectrum.h"

spectrum::spectrum(const char* node_name, void (*node_func)(void* arg), void (*prev_func)(void* arg), const int node_type)
{
    strcpy(m_name,node_name);
    
    // WARNING: DO NOT USE ARGUMENTS WHEN THE FUNCTION IS USED AS A DAEMON.
    // OR THE TICKER MAY LEAD TO UNDEFINED BEHAVIOUR.
    m_func = node_func;
    
    m_prev = prev_func;
    m_type = node_type;
    
    for(size_t i = 0; i != SUBNODE_MAX; i++)
        subnode_vec[i] = NULL;
    
    subnode_num = 0;
    m_parent = NULL;
    
    if(m_type == NODE_TYPE_DAEMON && m_func != NULL)
        m_ticker.attach((void(*)(void))m_func, DAEMON_PERIOD);  // The function pointer type compatibility is wierd in Ticker.
    
    if(s_rootflag)
    {
        s_nodenow = this;
        s_rootflag = 0;
    }
}

int spectrum::add_node(spectrum* node_ptr)
{
    if(subnode_num != SUBNODE_MAX)
    {
        node_ptr->m_parent = this;
        subnode_vec[subnode_num] = node_ptr;
        subnode_num++;
        return 0;
    }
    else
        return -1;
}

spectrum* spectrum::s_nodenow = NULL;   // Need to be initialized in constructor
int spectrum::s_rootflag = 1;   // Turned into 0 when the first node initalized
int spectrum::s_cursor = 0;
