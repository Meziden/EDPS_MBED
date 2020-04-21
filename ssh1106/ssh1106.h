#ifndef SSH1106_H
#define SSH1106_H

/*
 * SSH1106 encapsulation for mbed platform.
 * Powered by Peng Shuming, SWPU
 * The 64*132 RAM have already been mapped to 64*128 display and also framebuffer.
 */

const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 64;
const uint8_t SCREEN_PAGE = 8;

class ssh1106 : public SPI
{
    public:
    // Constructor
    ssh1106(PinName mosi, PinName sclk, PinName rst, PinName dc, PinName cs);
    
    // Text display - TODO: Adaptive PSR
    void draw_string(const char* str_buf, uint8_t arg_x = 0, uint8_t arg_y = 0);
    
    // Graphical Elements TODO: 
    void draw_pixel(int pos_x, int pos_y);
    void draw_rect(int pos_x, int pos_y, int width, int height);
    void draw_circle(int pos_x, int pos_y, float radius);
    void fill();
    void fill_rect(int pos_x, int pos_y, int width, int height);
    void fill_circle(int pos_x, int pos_y, float radius);
    
    // Settings
    void set_brightness(float value);

    // Send Framebuffer to Screen
    void commit();
    void commit(uint8_t page);
    
    // Framebuffer Operation
    void clear();
    void clear(uint8_t page);
    
    // 128*8*uint8 Framebuffer
    uint8_t m_framebuffer[128 * 8];
    
    private:
    void _command(uint8_t cmd);
    
    // Asynchronized Trasmission, Tx Complete Callback
    void _ready_clb(int);
    volatile bool m_ready;
    
    // Private pins to control ssh1106
    DigitalOut m_rst;
    DigitalOut m_dc;
    DigitalOut m_cs;
};

#endif
