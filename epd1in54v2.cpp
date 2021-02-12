/**
 *  @filename   :   epd1in54.cpp
 *  @brief      :   Implements for e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd1in54v2.h"

Epd::Epd(PinName mosi,
         PinName miso,
         PinName sclk,
         PinName cs,
         PinName dc,
         PinName rst,
         PinName busy) : EpdIf(mosi, miso, sclk, cs, dc, rst, busy)
{
}

Epd::Epd(SPI *spi,
         PinName cs,
         PinName dc,
         PinName rst,
         PinName busy) : EpdIf(spi, cs, dc, rst, busy)
{
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command) //
{
    DigitalWrite(m_dc, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data) //
{
    DigitalWrite(m_dc, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the m_busy goes HIGH
 */
void Epd::WaitUntilIdle(void) //
{
    int cnt{0};
    while (DigitalRead(m_busy) == 1 && ++cnt<2)
    { //0: busy, 1: idle
        DelayMs(10);
    }
    // DelayMs(200);
}

// int Epd::Init(const unsigned char *lut)
// {
//     /* EPD hardware init end */
//     return 0;
// }

int Epd::HDirInit(void) //
{
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0)
    {
        return -1;
    }
    /* EPD hardware init start */
    Reset();

    WaitUntilIdle();
    SendCommand(0x12); //SWRESET
    WaitUntilIdle();

    SendCommand(0x01); //Driver output control
    SendData(0xC7);
    SendData(0x00);
    SendData(0x01);

    SendCommand(0x11); //data entry mode
    SendData(0x01);

    SendCommand(0x44); //set Ram-X address start/end position
    SendData(0x00);
    SendData(0x18); //0x0C-->(18+1)*8=200

    SendCommand(0x45); //set Ram-Y address start/end position
    SendData(0xC7);    //0xC7-->(199+1)=200
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);

    SendCommand(0x3C); //BorderWavefrom
    SendData(0x01);

    SendCommand(0x18);
    SendData(0x80);

    SendCommand(0x22); // //Load Temperature and waveform setting.
    SendData(0XB1);
    SendCommand(0x20);

    SendCommand(0x4E); // set RAM x address count to 0;
    SendData(0x00);
    SendCommand(0x4F); // set RAM y address count to 0X199;
    SendData(0xC7);
    SendData(0x00);
    WaitUntilIdle();
    /* EPD hardware init end */

    return 0;
}

int Epd::LDirInit(void) //
{
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0)
    {
        return -1;
    }
    /* EPD hardware init start */
    Reset();

    WaitUntilIdle();
    SendCommand(0x12); //SWRESET
    WaitUntilIdle();

    SendCommand(0x01); //Driver output control
    SendData(0xC7);
    SendData(0x00);
    SendData(0x00);

    SendCommand(0x11); //data entry mode
    SendData(0x03);

    SendCommand(0x44);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((0 >> 3) & 0xFF);
    SendData((200 >> 3) & 0xFF);
    SendCommand(0x45);
    SendData(0 & 0xFF);
    SendData((0 >> 8) & 0xFF);
    SendData(200 & 0xFF);
    SendData((200 >> 8) & 0xFF);

    SendCommand(0x3C); //BorderWavefrom
    SendData(0x01);

    SendCommand(0x18);
    SendData(0x80);

    SendCommand(0x22); // //Load Temperature and waveform setting.
    SendData(0XB1);
    SendCommand(0x20);

    SendCommand(0x4E); // set RAM x address count to 0;
    SendData(0x00);
    SendCommand(0x4F); // set RAM y address count to 0X199;
    SendData(0xC7);
    SendData(0x00);
    WaitUntilIdle();
    /* EPD hardware init end */

    return 0;
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void) //
{
    DigitalWrite(m_rst, HIGH);
    DelayMs(200);
    DigitalWrite(m_rst, LOW); //module reset
    DelayMs(10);
    DigitalWrite(m_rst, HIGH);
    DelayMs(200);
}

void Epd::ClearDisplay(void) //
{
    int w, h;
    w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    h = EPD_HEIGHT;

    SendCommand(0x24);
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            SendData(0xff);
        }
    }
    //DISPLAY REFRESH
    DisplayFrame();
}

void Epd::Display(const unsigned char *frame_buffer) //
{
    int w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL)
    {
        SendCommand(0x24);
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    DisplayFrame();
}

void Epd::DisplayPartBaseImage(const unsigned char *frame_buffer) //
{
    int w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL)
    {
        SendCommand(0x24);
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                SendData(frame_buffer[i + j * w]);
            }
        }

        SendCommand(0x26);
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    DisplayFrame();
}
void Epd::DisplayPartBaseWhiteImage(void) //
{
    int w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    SendCommand(0x24);
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            SendData(0xff);
        }
    }

    SendCommand(0x26);
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            SendData(0xff);
        }
    }

    //DISPLAY REFRESH
    DisplayFrame();
}

void Epd::DisplayPart(const unsigned char *frame_buffer) //
{
    int w = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL)
    {
        SendCommand(0x24);
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    DisplayPartFrame();
}

/**
 *  @brief: set the look-up table register
 */
// void Epd::SetLut(const unsigned char *lut)
// {
//     SendCommand(WRITE_LUT_REGISTER);
//     /* the length of look-up table is 30 bytes */
//     for (int i = 0; i < 30; i++)
//     {
//         SendData(lut[i]);
//     }
// }

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 */
void Epd::SetFrameMemory(
    const unsigned char *image_buffer,
    int x,
    int y,
    int image_width,
    int image_height) //
{
    int x_end;
    int y_end;

    // DigitalWrite(m_rst, LOW); //module reset
    // DelayMs(10);
    // DigitalWrite(m_rst, HIGH);
    // DelayMs(10);
    // SendCommand(0x3c);
    // SendData(0x80);

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0)
    {
        return;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= this->width)
    {
        x_end = this->width - 1;
    }
    else
    {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= this->height)
    {
        y_end = this->height - 1;
    }
    else
    {
        y_end = y + image_height - 1;
    }
    SetMemoryArea(x, y, x_end, y_end);
    SetMemoryPointer(x, y);
    SendCommand(WRITE_RAM);
    /* send the image data */
    for (int j = 0; j < y_end - y + 1; j++)
    {
        for (int i = 0; i < (x_end - x + 1) / 8; i++)
        {
            SendData(image_buffer[i + j * (image_width / 8)]);
        }
    }
}

/**
 *  @brief: clear the frame memory with the specified color.
 *          this won't update the display.
 */
// void Epd::ClearFrameMemory(unsigned char color)
// {
//     SetMemoryArea(0, 0, this->width - 1, this->height - 1);
//     SetMemoryPointer(0, 0);
//     SendCommand(WRITE_RAM);
//     /* send the color data */
//     for (int i = 0; i < this->width / 8 * this->height; i++)
//     {
//         SendData(color);
//     }
// }

/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will 
 *          set the other memory area.
 */
void Epd::DisplayFrame(void) //
{
    SendCommand(DISPLAY_UPDATE_CONTROL_2);
    SendData(0xF7);
    // SendData(0xF4);
    SendCommand(MASTER_ACTIVATION);
    // SendCommand(TERMINATE_FRAME_READ_WRITE);
    WaitUntilIdle();
}

void Epd::DisplayPartFrame(void) //
{
    SendCommand(DISPLAY_UPDATE_CONTROL_2);
    SendData(0xFF);
    // SendData(0xFC);
    SendCommand(MASTER_ACTIVATION);
    WaitUntilIdle();
}


// void GxEPD2_154_D67::_Update_Full()
// {
//   _writeCommand(0x22);
//   _writeData(0xf4);
//   _writeCommand(0x20);
//   _waitWhileBusy("_Update_Full", full_refresh_time);
// }

// void GxEPD2_154_D67::_Update_Part()
// {
//   _writeCommand(0x22);
//   _writeData(0xfc);
//   _writeCommand(0x20);
//   _waitWhileBusy("_Update_Part", partial_refresh_time);
// }

/**
 *  @brief: private function to specify the memory area for data R/W
 */
void Epd::SetMemoryArea(int x_start, int y_start, int x_end, int y_end) //
{
    SendCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x_start >> 3) & 0xFF);
    SendData((x_end >> 3) & 0xFF);
    SendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
    SendData(y_start & 0xFF);
    SendData((y_start >> 8) & 0xFF);
    SendData(y_end & 0xFF);
    SendData((y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
void Epd::SetMemoryPointer(int x, int y) //
{
    SendCommand(SET_RAM_X_ADDRESS_COUNTER);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x >> 3) & 0xFF);
    SendCommand(SET_RAM_Y_ADDRESS_COUNTER);
    SendData(y & 0xFF);
    SendData((y >> 8) & 0xFF);
    WaitUntilIdle();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          You can use Epd::Init() to awaken
 */
void Epd::Sleep() //
{
    SendCommand(DEEP_SLEEP_MODE);
    SendData(0x01);
    WaitUntilIdle();

    DigitalWrite(m_rst, LOW);
}

/////////PAINT////////////

/**
 *  @brief: clear the image
 */
void Epd::ClearFramebuffer(unsigned char *frame_buffer, int colored)//
{
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->height; y++)
        {
            SetAbsolutePixel(frame_buffer, x, y, colored);
        }
    }
}

void Epd::SetRotate(int rotate)
{
    if (rotate == ROTATE_0)
    {
        rotate = ROTATE_0;
        width = EPD_WIDTH;
        height = EPD_HEIGHT;
    }
    else if (rotate == ROTATE_90)
    {
        rotate = ROTATE_90;
        width = EPD_HEIGHT;
        height = EPD_WIDTH;
    }
    else if (rotate == ROTATE_180)
    {
        rotate = ROTATE_180;
        width = EPD_WIDTH;
        height = EPD_HEIGHT;
    }
    else if (rotate == ROTATE_270)
    {
        rotate = ROTATE_270;
        width = EPD_HEIGHT;
        height = EPD_WIDTH;
    }
}

void Epd::SetPixel(unsigned char *frame_buffer, int x, int y, int colored)//
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }
    if (rotate == ROTATE_0)
    {
        SetAbsolutePixel(frame_buffer, x, y, colored);
    }
    else if (rotate == ROTATE_90)
    {
        int point_temp = x;
        x = EPD_WIDTH - y;
        y = point_temp;
        SetAbsolutePixel(frame_buffer, x, y, colored);
    }
    else if (rotate == ROTATE_180)
    {
        x = EPD_WIDTH - x;
        y = EPD_HEIGHT - y;
        SetAbsolutePixel(frame_buffer, x, y, colored);
    }
    else if (rotate == ROTATE_270)
    {
        int point_temp = x;
        x = y;
        y = EPD_HEIGHT - point_temp;
        SetAbsolutePixel(frame_buffer, x, y, colored);
    }
}

void Epd::SetAbsolutePixel(unsigned char *frame_buffer, int x, int y, int colored)//
{
    // To avoid display orientation effects
    // use EPD_WIDTH instead of self.width
    // use EPD_HEIGHT instead of self.height
    if (x < 0 || x >= EPD_WIDTH || y < 0 || y >= EPD_HEIGHT)
    {
        return;
    }

    if (colored)
    {
        frame_buffer[(x + y * EPD_WIDTH) / 8] &= ~(0x80 >> (x % 8));
    }
    else
    {
        frame_buffer[(x + y * EPD_WIDTH) / 8] |= 0x80 >> (x % 8);
    }

}

void Epd::DrawLine(unsigned char *frame_buffer, int x0, int y0, int x1, int y1, int colored)
{
    // Bresenham algorithm
    int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while ((x0 != x1) && (y0 != y1))
    {
        SetPixel(frame_buffer, x0, y0, colored);
        if (2 * err >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (2 * err <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Epd::DrawHorizontalLine(unsigned char *frame_buffer, int x, int y, int width, int colored)
{
    for (int i = x; i < x + width; i++)
    {
        SetPixel(frame_buffer, i, y, colored);
    }
}

void Epd::DrawVerticalLine(unsigned char *frame_buffer, int x, int y, int height, int colored)
{
    for (int i = y; i < y + height; i++)
    {
        SetPixel(frame_buffer, x, i, colored);
    }
}

void Epd::DrawRectangle(unsigned char *frame_buffer, int x0, int y0, int x1, int y1, int colored)
{
    int min_x = x1 > x0 ? x0 : x1;
    int max_x = x1 > x0 ? x1 : x0;
    int min_y = y1 > y0 ? y0 : y1;
    int max_y = y1 > y0 ? y1 : y0;
    DrawHorizontalLine(frame_buffer, min_x, min_y, max_x - min_x + 1, colored);
    DrawHorizontalLine(frame_buffer, min_x, max_y, max_x - min_x + 1, colored);
    DrawVerticalLine(frame_buffer, min_x, min_y, max_y - min_y + 1, colored);
    DrawVerticalLine(frame_buffer, max_x, min_y, max_y - min_y + 1, colored);
}

void Epd::DrawFilledRectangle(unsigned char *frame_buffer, int x0, int y0, int x1, int y1, int colored)
{
    int min_x = x1 > x0 ? x0 : x1;
    int max_x = x1 > x0 ? x1 : x0;
    int min_y = y1 > y0 ? y0 : y1;
    int max_y = y1 > y0 ? y1 : y0;

    for (int i = min_x; i < max_x + 1; i++)
    {
        DrawVerticalLine(frame_buffer, i, min_y, max_y - min_y + 1, colored);
    }
}

void Epd::DrawCircle(unsigned char *frame_buffer, int x, int y, int radius, int colored)
{
    // Bresenham algorithm
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    if (x >= width || y >= height)
    {
        return;
    }
    while (1)
    {
        SetPixel(frame_buffer, x - x_pos, y + y_pos, colored);
        SetPixel(frame_buffer, x + x_pos, y + y_pos, colored);
        SetPixel(frame_buffer, x + x_pos, y - y_pos, colored);
        SetPixel(frame_buffer, x - x_pos, y - y_pos, colored);
        int e2 = err;
        if (e2 <= y_pos)
        {
            y_pos += 1;
            err += y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos)
            {
                e2 = 0;
            }
        }
        if (e2 > x_pos)
        {
            x_pos += 1;
            err += x_pos * 2 + 1;
        }
        if (x_pos > 0)
        {
            break;
        }
    }
}

void Epd::DrawFilledCircle(unsigned char *frame_buffer, int x, int y, int radius, int colored)
{
    // Bresenham algorithm
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    if (x >= width || y >= height)
    {
        return;
    }
    while (1)
    {
        SetPixel(frame_buffer, x - x_pos, y + y_pos, colored);
        SetPixel(frame_buffer, x + x_pos, y + y_pos, colored);
        SetPixel(frame_buffer, x + x_pos, y - y_pos, colored);
        SetPixel(frame_buffer, x - x_pos, y - y_pos, colored);
        DrawHorizontalLine(frame_buffer, x + x_pos, y + y_pos, 2 * (-x_pos) + 1, colored);
        DrawHorizontalLine(frame_buffer, x + x_pos, y - y_pos, 2 * (-x_pos) + 1, colored);
        int e2 = err;
        if (e2 <= y_pos)
        {
            y_pos += 1;
            err += y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos)
            {
                e2 = 0;
            }
        }
        if (e2 > x_pos)
        {
            x_pos += 1;
            err += x_pos * 2 + 1;
        }
        if (x_pos > 0)
        {
            break;
        }
    }
}

/**
 *  @brief: this draws a charactor on the frame buffer but not refresh
 */
void Epd::DrawCharAt(unsigned char *frame_buffer, int x, int y, char ascii_char, sFONT *font, int colored)
{
    int i, j;
    unsigned int char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[char_offset];

    for (j = 0; j < font->Height; j++)
    {
        for (i = 0; i < font->Width; i++)
        {
            if (*ptr & (0x80 >> (i % 8)))
            {
                SetPixel(frame_buffer, x + i, y + j, colored);
            }
            if (i % 8 == 7)
            {
                ptr++;
            }
        }
        if (font->Width % 8 != 0)
        {
            ptr++;
        }
    }
}

/**
*  @brief: this displays a string on the frame buffer but not refresh
*/
void Epd::DrawStringAt(unsigned char *frame_buffer, int x, int y, const char *text, sFONT *font, int colored)
{
    const char *p_text = text;
    unsigned int counter = 0;
    int refcolumn = x;

    /* Send the string character by character on EPD */
    while (*p_text != 0)
    {
        /* Display one character on EPD */
        DrawCharAt(frame_buffer, refcolumn, y, *p_text, font, colored);
        /* Decrement the column position by 16 */
        refcolumn += font->Width;
        /* Point on the next character */
        p_text++;
        counter++;
    }
}
