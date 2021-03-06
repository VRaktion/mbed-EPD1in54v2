/**
 *  @filename   :   epd1in54.h
 *  @brief      :   Header file for e-paper display library epd1in54b.cpp
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

#ifndef EPD1IN54V2_H
#define EPD1IN54V2_H

#include "epdif.h"
#include "fonts.h"
#include "imagedata.h"

#define LOW 0
#define HIGH 1

// Display resolution
#define EPD_WIDTH 200
#define EPD_HEIGHT 200

#define COLORED 1
#define UNCOLORED 0

// EPD1IN54 commands
#define DRIVER_OUTPUT_CONTROL 0x01
#define BOOSTER_SOFT_START_CONTROL 0x0C
#define GATE_SCAN_START_POSITION 0x0F
#define DEEP_SLEEP_MODE 0x10
#define DATA_ENTRY_MODE_SETTING 0x11
#define SW_RESET 0x12
#define TEMPERATURE_SENSOR_CONTROL 0x1A
#define MASTER_ACTIVATION 0x20
#define DISPLAY_UPDATE_CONTROL_1 0x21
#define DISPLAY_UPDATE_CONTROL_2 0x22
#define WRITE_RAM 0x24
#define WRITE_VCOM_REGISTER 0x2C
#define WRITE_LUT_REGISTER 0x32
#define SET_DUMMY_LINE_PERIOD 0x3A
#define SET_GATE_TIME 0x3B
#define BORDER_WAVEFORM_CONTROL 0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION 0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION 0x45
#define SET_RAM_X_ADDRESS_COUNTER 0x4E
#define SET_RAM_Y_ADDRESS_COUNTER 0x4F
#define TERMINATE_FRAME_READ_WRITE 0xFF

// Display orientation
#define ROTATE_0 0
#define ROTATE_90 1
#define ROTATE_180 2
#define ROTATE_270 3

#define IF_INVERT_COLOR     1
 
extern const unsigned char lut_full_update[];
extern const unsigned char lut_partial_update[];

class Epd : EpdIf
{
public:
    unsigned long width;
    unsigned long height;
    unsigned int rotate;

    Epd(PinName mosi,
        PinName miso,
        PinName sclk,
        PinName cs,
        PinName dc,
        PinName rst,
        PinName busy);
    ~Epd();
    // int  Init(const unsigned char* lut);
    int LDirInit(void);//new
    int HDirInit(void);//new
    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void WaitUntilIdle(void);
    void Reset(void);
    void ClearDisplay(void);//new

    void Display(const unsigned char *frame_buffer);//new
    void DisplayPartBaseImage(const unsigned char *frame_buffer);//new
    void DisplayPartBaseWhiteImage(void);//new
    void DisplayPart(const unsigned char *frame_buffer);//new

    // void SetLut(const unsigned char* lut);

    void SetFrameMemory(
        const unsigned char *image_buffer,
        int x,
        int y,
        int image_width,
        int image_height);
    // void ClearFrameMemory(unsigned char color);

    void DisplayFrame(void);
    void DisplayPartFrame(void);

    void Sleep(void);

    //Paint

    void ClearFramebuffer(unsigned char *frame_buffer, int colored);
    void SetRotate(int rotate);
    void SetPixel(unsigned char *frame_buffer, int x, int y, int colored);
    void SetAbsolutePixel(unsigned char *frame_buffer, int x, int y, int colored);
    void DrawLine(unsigned char *frame_buffer, int x0, int y0, int x1, int y1, int colored);
    void DrawHorizontalLine(unsigned char *frame_buffer, int x, int y, int width, int colored);
    void DrawVerticalLine(unsigned char *frame_buffer, int x, int y, int height, int colored);
    void DrawRectangle(unsigned char *frame_buffer, int x0, int y0, int x1, int y1, int colored);
    void DrawFilledRectangle(unsigned char *frame_buffer, int x0, int y0, int x1, int y1, int colored);
    void DrawCircle(unsigned char *frame_buffer, int x, int y, int radius, int colored);
    void DrawFilledCircle(unsigned char *frame_buffer, int x, int y, int radius, int colored);
    void DrawCharAt(unsigned char *frame_buffer, int x, int y, char ascii_char, sFONT *font, int colored);
    void DrawStringAt(unsigned char *frame_buffer, int x, int y, const char *text, sFONT *font, int colored);

private:
    void SetMemoryArea(int x_start, int y_start, int x_end, int y_end);
    void SetMemoryPointer(int x, int y);
};

#endif /* EPD1IN54V2_H */

/* END OF FILE */