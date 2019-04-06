#include "mbed.h"
#include "ssh1106.h"
#include "font_8x8_1.h"

ssh1106::ssh1106(PinName mosi, PinName miso, PinName sclk, PinName rst, PinName dc, PinName cs):SPI(mosi, miso, sclk),
                                                                                                m_rst(rst),
                                                                                                m_dc(dc),
                                                                                                m_cs(cs)
{
    // Reset SSH1106
    m_rst = 0;
    wait_ms(100);
    m_rst = 1;
    
    //Init Configuration, Different from SSD1306, not clear about this part.
    _command(0xAE);
    _command(0x02);
    _command(0x10);
    _command(0x40);
    _command(0xB0);
    _command(0x81);
    _command(0x80);
    _command(0xA1);
    _command(0xA6);
    _command(0xA8);
    _command(0x3F);
    _command(0xAD);
    _command(0x8B);
    _command(0xC8);
    _command(0xD3);
    _command(0x00);
    _command(0xD5);
    _command(0x80);
    _command(0xD9);
    _command(0x1F);
    _command(0xDA);
    _command(0x12);
    _command(0xDB);
    _command(0x40);
    
    // Open Display
    _command(0xAF);
}

void ssh1106::draw_string(const char* str_buf, uint8_t arg_x, uint8_t arg_y)
{
    int bufpos = 0;
    while(str_buf[0] != '\0')
    {
        for(uint8_t i = 0; i != 8; i++)
        {
            m_framebuffer[arg_y * 128 + arg_x * 8 + bufpos + i] = font_8x8_1[(uint8_t)(*str_buf) * 8 + i + 8];
        }
        str_buf += 1;
        bufpos += 8;
    }
}

void ssh1106::draw_pixel(int pos_x, int pos_y)
{
    // Avoid Stack Violation
    if(pos_x < SCREEN_WIDTH && pos_y < SCREEN_HEIGHT && pos_x >= 0 && pos_y >= 0)
        m_framebuffer[(pos_y/8)*128 + pos_x] |= 1<<(pos_y%8);
}

void ssh1106::draw_rect(int pos_x, int pos_y, int width, int height)
{
    // Draw Horizontal Lines
    for(int tmp_x = 0; tmp_x != width; tmp_x++)
    {
        draw_pixel(pos_x + tmp_x, pos_y);
        draw_pixel(pos_x + tmp_x, pos_y + height -1);
    }
    // Draw Vertial Lines
    for(int tmp_y = 0; tmp_y != height; tmp_y++)
    {
        draw_pixel(pos_x, pos_y + tmp_y);
        draw_pixel(pos_x + width -1, pos_y + tmp_y);
    }
}

void ssh1106::draw_circle(int pos_x, int pos_y, float radius)
{
    for(int tmp_x = (int)floor(pos_x - radius)-2; tmp_x != (int)ceil(pos_x + radius) +2 ; tmp_x++)
    {
        for(int tmp_y = (int)floor(pos_y - radius)-2; tmp_y != (int)ceil(pos_y + radius) +2; tmp_y++)
        {
            float distance = sqrt((double)((tmp_x - pos_x) * (tmp_x - pos_x) + (tmp_y - pos_y) * (tmp_y - pos_y)));
            if(distance - radius <= 0.57 && distance - radius >= -0.57)
                draw_pixel(tmp_x, tmp_y);
        }
    }
}

void ssh1106::fill()
{
    memset(m_framebuffer, 0xFF, SCREEN_WIDTH * SCREEN_PAGE);
}

void ssh1106::fill_rect(int pos_x, int pos_y, int width, int height)
{
    // Draw Horizontal Lines
    for(int tmp_x = 0; tmp_x != width; tmp_x++)
    {
        for(int tmp_y = 0; tmp_y != height; tmp_y++)
        {
            draw_pixel(pos_x + tmp_x, pos_y + tmp_y);
        }
    }
}

void ssh1106::fill_circle(int pos_x, int pos_y, float radius)
{
    for(int tmp_x = (int)floor(pos_x - radius)-2; tmp_x != (int)ceil(pos_x + radius) +2 ; tmp_x++)
    {
        for(int tmp_y = (int)floor(pos_y - radius)-2; tmp_y != (int)ceil(pos_y + radius) +2; tmp_y++)
        {
            float distance = sqrt((double)((tmp_x - pos_x) * (tmp_x - pos_x) + (tmp_y - pos_y) * (tmp_y - pos_y)));
            if(radius - distance >= -0.57)
                draw_pixel(tmp_x, tmp_y);
        }
    }
}

void ssh1106::commit()
{
    for(uint8_t y = 0; y != SCREEN_PAGE; y++)
    {
        m_dc = 0;
        write((uint8_t)0xb0 + y);
        write((uint8_t)0x10);
        write((uint8_t)0x00);
        m_dc = 1;
        // Offset
        write(0x00);
        write(0x00);
        for(size_t x = 0; x != 128; x++)
        {
            write(m_framebuffer[128 * y + x]);
        }
    }
}

void ssh1106::commit(uint8_t page)
{
    if(page < SCREEN_PAGE)
    {
        m_dc = 0;
        write((uint8_t)0xb0 + page);
        write((uint8_t)0x10);
        write((uint8_t)0x00);
        m_dc = 1;
        // Offset
        write(0x00);
        write(0x00);
        for(size_t x = 0; x != 128; x++)
        {
            write(m_framebuffer[128 * page + x]);
        }
    }
}

void ssh1106::clear()
{
    memset(m_framebuffer, 0x00, SCREEN_WIDTH * SCREEN_PAGE);
}

void ssh1106::clear(uint8_t page)
{
    if(page < SCREEN_PAGE)
        memset(m_framebuffer + SCREEN_WIDTH * page, 0x00, SCREEN_WIDTH);
}

//Private:

void ssh1106::_command(uint8_t cmd)
{
    m_dc = 0;
    write(cmd);
}
