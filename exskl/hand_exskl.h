/* 
 * Hand Exoskeleton System - Software Adaption on ARM mbed Platform
 * Powered by Liu Aiheng and Wang Fangjie
 * With Built-In PID-FSM Combined Control Logic
 * 20190731-v3.0
 */

#ifndef HAND_EXSKL_H
#define HAND_EXSKL_H

// PID Parameters
const float PID_PARAMETER_P = 2.0f;     // < 5.0 recommended
const float PID_PARAMETER_I = 0.05f;    // < 0.2 recommended
const float PID_PARAMETER_D = 0.1f;
const float PID_FREQUENCY   = 50.0f;    // < 100 recommended on MCUs without fpu.

// FSM States Definitions
const int STATE_BEND    = 0x01;
const int STATE_UNBEND  = 0x02;
const int STATE_IDLE    = 0x00;

class hand_exskl : public Ticker
{
    public:
    // Constructor
    hand_exskl( PinName press,              // AnalogIn for Bidirectional Finger Pressure Detector
                PinName tense_top,          // AnalogIn for Motor Drive Tension Detector - Top
                PinName tense_bottom,       // AnalogIn for Motor Drive Tension Detector - Bottom
                PinName motorpwm_top,       // Motor on Top - PWM Control
                PinName motorctl1_top,      // Motor on Top - Mode Control 1
                PinName motorctl2_top,      // Motor on Top - Mode Control 2
                PinName motorpwm_bot,       // Motor on Bottom - PWM Control
                PinName motorctl1_bot,      // Motor on Bottom - Mode Control 1
                PinName motorctl2_bot);     // Motor on Bottom - Mode Control 2
    
    private:
    // Sensors - Analog Multiplexer is Recommended to reduce the complexity.
    AnalogIn m_sensor_press;        // Bend Trend > 0.5, Unbend Trend < 0.5, Use 10k Ohm resistors. 
    AnalogIn m_sensor_top;          // When Tensed < 1
    AnalogIn m_sensor_bottom;       // When Tensed < 1
    
    // Motor Drives
    // Motor Bend Operation = (0~1)
    // Motor Unbend Operation = (-1~0) 
    xy160d m_motor_top, m_motor_bottom;
    
    // Control Signal Buffer, for IIR Filter
    float motor_ctl_top, motor_ctl_bottom;
    
    // Built-In PID-FSM Controller
    void scheduler();
    // PID Error
    float m_error_p, m_error_i, m_error_d;
    // Finite-State-Machine
    int m_state;
};

#endif
