/*
 * Discrete Time PID Controller for ARM mbed Platform
 * based on mbed::Ticker for asynchronized operation.
 */

#ifndef PID_H
#define PID_H

// Callback Function Typedefs
typedef float(*piderr_clb_t)(void);
typedef void(*pidctl_clb_t)(float);

class PID : public Ticker
{
    public:
    // Constructor
    PID(piderr_clb_t err_clb,
        pidctl_clb_t ctl_clb,
        float par_p = 1.0f,
        float par_i = 0.0f,
        float par_d = 0.0f,
        float frequency = 50.0f);
    
    // PID Parameters and Internal Data
    float m_parameter_p;
    float m_parameter_i;
    float m_parameter_d;

    private:
    // Scheduler for PID
    void scheduler();
    // PID Error
    float m_error_p, m_error_i, m_error_d;

    // Functions to be called to get error and set control signal:
    piderr_clb_t m_geterr;
    pidctl_clb_t m_setctl;
};

#endif
