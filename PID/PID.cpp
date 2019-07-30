#include "mbed.h"
#include "PID.h"

PID::PID(piderr_clb_t err_clb,
        pidctl_clb_t ctl_clb,
        float par_p,
        float par_i,
        float par_d,
        float frequency):
        m_parameter_p(par_p),
        m_parameter_i(par_i),
        m_parameter_d(par_d)
{
    // Setting up Callback Functions
    m_geterr = err_clb;
    m_setctl = ctl_clb;

    // Start Processing
    attach(callback(this, &PID::scheduler), 1.0f/frequency);
}

void PID::scheduler()
{
    // Error now
    float error_tmp = m_geterr();

    // Update Internal Parameters
    m_error_d = error_tmp - m_error_p;
    m_error_i += error_tmp;
    m_error_p = error_tmp;

    float value_ctl =   m_error_p * m_parameter_p +
                        m_error_i * m_parameter_i +
                        m_error_d * m_parameter_d;

    (*m_setctl)(value_ctl);
}
