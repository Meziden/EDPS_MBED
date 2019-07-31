#include "mbed.h"
#include "xy160d.h"
#include "hand_exskl.h"

// Constructor
hand_exskl::hand_exskl( PinName press,              // AnalogIn for Bidirectional Finger Pressure Detector
                        PinName tense_top,          // AnalogIn for Motor Drive Tension Detector - Top
                        PinName tense_bottom,       // AnalogIn for Motor Drive Tension Detector - Bottom
                        PinName motorpwm_top,       // Motor on Top - PWM Control
                        PinName motorctl1_top,      // Motor on Top - Mode Control 1
                        PinName motorctl2_top,      // Motor on Top - Mode Control 2
                        PinName motorpwm_bot,       // Motor on Bottom - PWM Control
                        PinName motorctl1_bot,      // Motor on Bottom - Mode Control 1
                        PinName motorctl2_bot):     // Motor on Bottom - Mode Control 2
                        m_sensor_press(press),
                        m_sensor_top(tense_top),
                        m_sensor_bottom(tense_bottom),
                        m_motor_top(motorpwm_top, motorctl1_top, motorctl2_top),
                        m_motor_bottom(motorpwm_bot, motorctl1_bot, motorctl2_bot)
{
    // Initializing
    motor_ctl_top = motor_ctl_bottom = 0;
    m_error_p = m_error_i = m_error_d = 0.0f;
    m_state = STATE_IDLE;
    
    // Start Scheduler
    attach(callback(this, &hand_exskl::scheduler), PID_FREQUENCY);
}

void hand_exskl::scheduler()
{
    // Main PID Control---------------------------------------------------------
    // Bend Trend: Error > 0
    // Unbend Trend: Error < 0
    float error_tmp = m_sensor_press - 0.5f;

    // Update Internal Parameters
    m_error_d = error_tmp - m_error_p;
    m_error_i += error_tmp;
    m_error_p = error_tmp;
    
    // Main motor drive control signal
    float motor_ctl =   m_error_p * PID_PARAMETER_P +
                        m_error_i * PID_PARAMETER_I +
                        m_error_d * PID_PARAMETER_D;
    
    // State Encoding and Transition Handling-----------------------------------
    if(error_tmp > -0.05f || error_tmp < 0.05f)
    {
        // State Transition from BEND/UNBEND
        if(m_state == STATE_BEND)
        {
            // Reset bottom motor control signal govenored by PID, use IIR instead.
            motor_ctl_bottom = 0.0f;
        }
        if(m_state == STATE_UNBEND)
        {
            // Reset top motor control signal govenored by PID, use IIR instead.
            motor_ctl_top = 0.0f;
        }
        // PID is not involved in IDLE mode.
        m_state = STATE_IDLE;
    }
    else if(error_tmp >= 0.05f)
    {
        // State Transition from UNBEND
        if(m_state == STATE_UNBEND)
        {
            // Reset PID, avoid oscillation.
            motor_ctl = m_error_p = m_error_i = m_error_d = 0.0f;
            
            // Reset control signal status.
            motor_ctl_top = motor_ctl_bottom = 0.0f;
        }
        if(m_state == STATE_IDLE)
        {
            // Reset PID, avoid oscillation.
            motor_ctl = m_error_p = m_error_i = m_error_d = 0.0f;
            
            // Reset bottom motor control signal govenored by IIR, use PID instead.
            motor_ctl_bottom = 0.0f;
        }
        m_state = STATE_BEND;
    }
    else
    {
        // State Transition from UNBEND
        if(m_state == STATE_BEND)
        {
            // Reset PID, avoid oscillation.
            motor_ctl = m_error_p = m_error_i = m_error_d = 0.0f;
            
            // Reset control signal status.
            motor_ctl_top = motor_ctl_bottom = 0.0f;
        }
        if(m_state == STATE_IDLE)
        {
            // Reset PID, avoid oscillation.
            motor_ctl = m_error_p = m_error_i = m_error_d = 0.0f;
            
            // Reset top motor control signal govenored by IIR, use PID instead.
            motor_ctl_top = 0.0f;
        }
        m_state = STATE_UNBEND;
    }
    
    // State Decoding-----------------------------------------------------------
    switch(m_state)
    {
        case STATE_IDLE:
            // Make the tensiometer slightly tensed.
            // IIR involved to reduce oscillation and sudden load on motor drives.
            if(m_sensor_top > 0.95f) // Loose
                motor_ctl_top = 0.9f * motor_ctl_top + 0.1f * (1.0f - motor_ctl_top);
            if(m_sensor_top < 0.75f) // Tight
                motor_ctl_top = 0.9f * motor_ctl_top + 0.1f * (-1.0f + motor_ctl_top);
            
            if(m_sensor_bottom > 0.95f) //Loose
                motor_ctl_bottom = 0.9f * motor_ctl_bottom + 0.1f * (1.0f - motor_ctl_bottom);
            if(m_sensor_bottom < 0.75f) //Tight
                motor_ctl_bottom = 0.9f * motor_ctl_bottom + 0.1f * (-1.0f + motor_ctl_bottom);
            
            break;
        
        case STATE_BEND:
            // Make the tensiometer slightly tensed.
            // IIR involved to reduce oscillation and sudden load on motor drives.
            if(m_sensor_top > 0.95f) // Loose
                motor_ctl_top = 0.9f * motor_ctl_top + 0.1f * (1.0f - motor_ctl_top);
            if(m_sensor_top < 0.75f) // Tight
                motor_ctl_top = 0.9f * motor_ctl_top + 0.1f * (-1.0f + motor_ctl_top);
            
            // Main Motor Drive in Bend Operation, Governed by PID
            motor_ctl_bottom = motor_ctl;    
            
            break;
        
        case STATE_UNBEND:
            // Main Motor Drive in Unbend Operation, Governed by PID
            motor_ctl_top = motor_ctl; 
            
            // Make the tensiometer slightly tensed.
            // IIR involved to reduce oscillation and sudden load on motor drives.
            if(m_sensor_bottom > 0.95f) //Loose
                motor_ctl_bottom = 0.9f * motor_ctl_bottom + 0.1f * (1.0f - motor_ctl_bottom);
            if(m_sensor_bottom < 0.75f) //Tight
                motor_ctl_bottom = 0.9f * motor_ctl_bottom + 0.1f * (-1.0f + motor_ctl_bottom);
                
            break;
    }
    
    // Send Motor Control Signal -----------------------------------------------
    m_motor_top = motor_ctl_top;
    m_motor_bottom = motor_ctl_bottom;
}
