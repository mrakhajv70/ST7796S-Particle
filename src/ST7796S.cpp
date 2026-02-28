/**
 * @file ST7796S.cpp
 * @brief ST7796S TFT Display Driver for Particle Photon 2
 * @see ST7796S.h for full documentation and usage
 *
 * @author NC State SolarPack — estyntech
 * @version 1.0.0
 * @date 2026
 * @license MIT
 */

#include "ST7796S.h"
#include "SPI.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ============================================================
// 5×7 Bitmap Font (ASCII 32–126)
//
// Each entry is 5 bytes representing 5 vertical columns.
// Within each byte, bit 0 = top pixel, bit 6 = bottom pixel.
// Characters are rendered 5px wide × 7px tall, with 1px spacing
// on the right and bottom, giving a 6×8 cell per character.
// ============================================================
static const uint8_t FONT5X7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '  32
    {0x00,0x00,0x5F,0x00,0x00}, // '!'  33
    {0x00,0x07,0x00,0x07,0x00}, // '"'  34
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'  35
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'  36
    {0x23,0x13,0x08,0x64,0x62}, // '%'  37
    {0x36,0x49,0x55,0x22,0x50}, // '&'  38
    {0x00,0x05,0x03,0x00,0x00}, // '\'' 39
    {0x00,0x1C,0x22,0x41,0x00}, // '('  40
    {0x00,0x41,0x22,0x1C,0x00}, // ')'  41
    {0x14,0x08,0x3E,0x08,0x14}, // '*'  42
    {0x08,0x08,0x3E,0x08,0x08}, // '+'  43
    {0x00,0x50,0x30,0x00,0x00}, // ','  44
    {0x08,0x08,0x08,0x08,0x08}, // '-'  45
    {0x00,0x60,0x60,0x00,0x00}, // '.'  46
    {0x20,0x10,0x08,0x04,0x02}, // '/'  47
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'  48
    {0x00,0x42,0x7F,0x40,0x00}, // '1'  49
    {0x42,0x61,0x51,0x49,0x46}, // '2'  50
    {0x21,0x41,0x45,0x4B,0x31}, // '3'  51
    {0x18,0x14,0x12,0x7F,0x10}, // '4'  52
    {0x27,0x45,0x45,0x45,0x39}, // '5'  53
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'  54
    {0x01,0x71,0x09,0x05,0x03}, // '7'  55
    {0x36,0x49,0x49,0x49,0x36}, // '8'  56
    {0x06,0x49,0x49,0x29,0x1E}, // '9'  57
    {0x00,0x36,0x36,0x00,0x00}, // ':'  58
    {0x00,0x56,0x36,0x00,0x00}, // ';'  59
    {0x08,0x14,0x22,0x41,0x00}, // '<'  60
    {0x14,0x14,0x14,0x14,0x14}, // '='  61
    {0x00,0x41,0x22,0x14,0x08}, // '>'  62
    {0x02,0x01,0x51,0x09,0x06}, // '?'  63
    {0x32,0x49,0x79,0x41,0x3E}, // '@'  64
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'  65
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'  66
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'  67
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'  68
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'  69
    {0x7F,0x09,0x09,0x09,0x01}, // 'F'  70
    {0x3E,0x41,0x49,0x49,0x7A}, // 'G'  71
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'  72
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'  73
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'  74
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'  75
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'  76
    {0x7F,0x02,0x0C,0x02,0x7F}, // 'M'  77
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'  78
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'  79
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'  80
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'  81
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'  82
    {0x46,0x49,0x49,0x49,0x31}, // 'S'  83
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'  84
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'  85
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'  86
    {0x3F,0x40,0x38,0x40,0x3F}, // 'W'  87
    {0x63,0x14,0x08,0x14,0x63}, // 'X'  88
    {0x07,0x08,0x70,0x08,0x07}, // 'Y'  89
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'  90
    {0x00,0x7F,0x41,0x41,0x00}, // '['  91
    {0x02,0x04,0x08,0x10,0x20}, // '\\' 92
    {0x00,0x41,0x41,0x7F,0x00}, // ']'  93
    {0x04,0x02,0x01,0x02,0x04}, // '^'  94
    {0x40,0x40,0x40,0x40,0x40}, // '_'  95
    {0x00,0x01,0x02,0x04,0x00}, // '`'  96
    {0x20,0x54,0x54,0x54,0x78}, // 'a'  97
    {0x7F,0x48,0x44,0x44,0x38}, // 'b'  98
    {0x38,0x44,0x44,0x44,0x20}, // 'c'  99
    {0x38,0x44,0x44,0x48,0x7F}, // 'd' 100
    {0x38,0x54,0x54,0x54,0x18}, // 'e' 101
    {0x08,0x7E,0x09,0x01,0x02}, // 'f' 102
    {0x0C,0x52,0x52,0x52,0x3E}, // 'g' 103
    {0x7F,0x08,0x04,0x04,0x78}, // 'h' 104
    {0x00,0x44,0x7D,0x40,0x00}, // 'i' 105
    {0x20,0x40,0x44,0x3D,0x00}, // 'j' 106
    {0x7F,0x10,0x28,0x44,0x00}, // 'k' 107
    {0x00,0x41,0x7F,0x40,0x00}, // 'l' 108
    {0x7C,0x04,0x18,0x04,0x78}, // 'm' 109
    {0x7C,0x08,0x04,0x04,0x78}, // 'n' 110
    {0x38,0x44,0x44,0x44,0x38}, // 'o' 111
    {0x7C,0x14,0x14,0x14,0x08}, // 'p' 112
    {0x08,0x14,0x14,0x18,0x7C}, // 'q' 113
    {0x7C,0x08,0x04,0x04,0x08}, // 'r' 114
    {0x48,0x54,0x54,0x54,0x20}, // 's' 115
    {0x04,0x3F,0x44,0x40,0x20}, // 't' 116
    {0x3C,0x40,0x40,0x40,0x7C}, // 'u' 117
    {0x1C,0x20,0x40,0x20,0x1C}, // 'v' 118
    {0x3C,0x40,0x30,0x40,0x3C}, // 'w' 119
    {0x44,0x28,0x10,0x28,0x44}, // 'x' 120
    {0x0C,0x50,0x50,0x50,0x3C}, // 'y' 121
    {0x44,0x64,0x54,0x4C,0x44}, // 'z' 122
    {0x00,0x08,0x36,0x41,0x00}, // '{' 123
    {0x00,0x00,0x7F,0x00,0x00}, // '|' 124
    {0x00,0x41,0x36,0x08,0x00}, // '}' 125
    {0x10,0x08,0x08,0x10,0x08}, // '~' 126
};


// ============================================================
// Constructor
// ============================================================
ST7796S::ST7796S(uint8_t csPin, uint8_t dcPin, uint8_t rstPin)
    : _cs(csPin), _dc(dcPin), _rst(rstPin),
      _width(ST7796_NATIVE_WIDTH),
      _height(ST7796_NATIVE_HEIGHT),
      _cursorX(0), _cursorY(0),
      _textColor(ST7796_WHITE),
      _textBgColor(ST7796_BLACK),
      _useTextBg(false),
      _textSize(1),
      _rotation(0)
{}


// ============================================================
// begin()
// ============================================================
void ST7796S::begin() {
    pinMode(_cs,  OUTPUT);
    pinMode(_dc,  OUTPUT);
    pinMode(_rst, OUTPUT);

    // Deselect display and set known pin states
    digitalWrite(_cs,  HIGH);
    digitalWrite(_rst, HIGH);
    digitalWrite(_dc,  HIGH);

    SPI.begin();
    SPI.setClockSpeed(ST7796_SPI_MHZ, MHZ);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    initST7796S();
}


// ============================================================
// Low-level SPI
// ============================================================

void ST7796S::writeCommand(uint8_t cmd) {
    digitalWrite(_dc, LOW);
    digitalWrite(_cs, LOW);
    SPI.transfer(cmd);
    digitalWrite(_cs, HIGH);
}

void ST7796S::writeData(uint8_t data) {
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(data);
    digitalWrite(_cs, HIGH);
}

void ST7796S::writeData16(uint16_t data) {
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(data >> 8);
    SPI.transfer(data & 0xFF);
    digitalWrite(_cs, HIGH);
}

void ST7796S::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(0x2A); // Column Address Set (CASET)
    writeData(x0 >> 8); writeData(x0 & 0xFF);
    writeData(x1 >> 8); writeData(x1 & 0xFF);

    writeCommand(0x2B); // Row Address Set (RASET)
    writeData(y0 >> 8); writeData(y0 & 0xFF);
    writeData(y1 >> 8); writeData(y1 & 0xFF);

    writeCommand(0x2C); // Memory Write (RAMWR)
}


// ============================================================
// ST7796S Initialization Sequence
//
// Derived from the official ST7796S datasheet and validated
// against the MSP4031 module (lcdwiki.com/4.0inch_Capacitive_SPI_Module_ST7796).
//
// Key decisions:
//   0x3A / 0x55 = 16-bit RGB565 pixel format
//   0x36 / 0x28 = MADCTL: MX=1, BGR=1 (correct orientation/colors for this module)
//   0x21 (Display Inversion ON) is intentionally OMITTED — it inverts all colors
//   0x29 = Display ON
// ============================================================
void ST7796S::initST7796S() {
    // Hardware reset
    digitalWrite(_rst, HIGH); delay(50);
    digitalWrite(_rst, LOW);  delay(100);
    digitalWrite(_rst, HIGH); delay(150);

    writeCommand(0x01); delay(120); // Software Reset — wait 120ms per datasheet
    writeCommand(0x11); delay(120); // Sleep Out     — wait 120ms per datasheet

    writeCommand(0x3A); writeData(0x55); // Pixel Format: 16-bit RGB565

    // Memory Access Control
    // 0x28 = MX bit (column address reversed) + BGR bit
    // This combination produces correct landscape orientation with correct colors
    // on the MSP4031 module. Do not change without retesting color/rotation.
    writeCommand(0x36); writeData(0x28);

    // Display Function Control
    writeCommand(0xB6);
    writeData(0x00);
    writeData(0x02);
    writeData(0x3B);

    // Power Control 1
    writeCommand(0xC0); writeData(0x80); writeData(0x64);

    // Power Control 2
    writeCommand(0xC1); writeData(0x13);

    // Power Control 3
    writeCommand(0xC2); writeData(0xA7);

    // VCOM Control
    writeCommand(0xC5); writeData(0x09);
    delay(120);

    // Positive Gamma Correction
    writeCommand(0xE0);
    writeData(0xF0); writeData(0x00); writeData(0x0A); writeData(0x10);
    writeData(0x12); writeData(0x1B); writeData(0x39); writeData(0x44);
    writeData(0x47); writeData(0x28); writeData(0x12); writeData(0x10);
    writeData(0x16); writeData(0x1B);

    // Negative Gamma Correction
    writeCommand(0xE1);
    writeData(0xF0); writeData(0x00); writeData(0x0A); writeData(0x10);
    writeData(0x11); writeData(0x1A); writeData(0x3B); writeData(0x34);
    writeData(0x4E); writeData(0x3A); writeData(0x17); writeData(0x16);
    writeData(0x21); writeData(0x22);

    // NOTE: Command 0x21 (Display Inversion ON) is deliberately NOT sent here.
    // Sending 0x21 inverts all pixel colors (bitwise NOT on RGB565 values),
    // causing red to appear cyan, green to appear magenta, etc.
    // This module does not require inversion for correct color output.

    writeCommand(0x29); // Display ON
    delay(50);
}


// ============================================================
// setRotation
//
// MADCTL register (0x36) bit layout for ST7796S:
//   Bit 7 (0x80) = MY  — row address order
//   Bit 6 (0x40) = MX  — column address order
//   Bit 5 (0x20) = MV  — row/column exchange (transpose)
//   Bit 4 (0x10) = ML  — vertical refresh direction
//   Bit 3 (0x08) = BGR — swap red and blue
//   Bit 2 (0x04) = MH  — horizontal refresh direction
//
// Verified MADCTL values for MSP4031 module:
//   Rotation 0 (480×320, landscape):           0x28
//   Rotation 1 (320×480, portrait, CW 90°):    0xA8
//   Rotation 2 (480×320, landscape, flipped):  0xC8
//   Rotation 3 (320×480, portrait, CCW 90°):   0x68
// ============================================================
void ST7796S::setRotation(uint8_t r) {
    _rotation = r % 4;
    writeCommand(0x36);
    switch (_rotation) {
        case 0:
            writeData(0x28);
            _width  = ST7796_NATIVE_WIDTH;
            _height = ST7796_NATIVE_HEIGHT;
            break;
        case 1:
            writeData(0xA8);
            _width  = ST7796_NATIVE_HEIGHT;
            _height = ST7796_NATIVE_WIDTH;
            break;
        case 2:
            writeData(0xC8);
            _width  = ST7796_NATIVE_WIDTH;
            _height = ST7796_NATIVE_HEIGHT;
            break;
        case 3:
            writeData(0x68);
            _width  = ST7796_NATIVE_HEIGHT;
            _height = ST7796_NATIVE_WIDTH;
            break;
    }
}


// ============================================================
// Screen & Rectangle
// ============================================================

void ST7796S::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

void ST7796S::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (w == 0 || h == 0) return;
    if (x >= _width || y >= _height) return;

    // Clamp to display bounds
    if (x + w > _width)  w = _width  - x;
    if (y + h > _height) h = _height - y;

    setWindow(x, y, x + w - 1, y + h - 1);
    uint8_t hi = color >> 8, lo = color & 0xFF;

    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        SPI.transfer(hi);
        SPI.transfer(lo);
    }
    digitalWrite(_cs, HIGH);
}

void ST7796S::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawHLine(x, y,         w, color); // top
    drawHLine(x, y + h - 1, w, color); // bottom
    drawVLine(x,         y, h, color); // left
    drawVLine(x + w - 1, y, h, color); // right
}

void ST7796S::drawBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint16_t color, uint8_t thickness) {
    for (uint8_t i = 0; i < thickness; i++) {
        if (w <= 2*i || h <= 2*i) break; // prevent underflow
        drawRect(x + i, y + i, w - 2*i, h - 2*i, color);
    }
}


// ============================================================
// Pixels & Lines
// ============================================================

void ST7796S::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= _width || y >= _height) return;
    setWindow(x, y, x, y);
    digitalWrite(_dc, HIGH);
    digitalWrite(_cs, LOW);
    SPI.transfer(color >> 8);
    SPI.transfer(color & 0xFF);
    digitalWrite(_cs, HIGH);
}

void ST7796S::drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    fillRect(x, y, w, 1, color);
}

void ST7796S::drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    fillRect(x, y, 1, h, color);
}

void ST7796S::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    // Optimise axis-aligned lines
    if (y0 == y1) { drawHLine(x0, y0, abs(x1-x0)+1, color); return; }
    if (x0 == x1) { drawVLine(x0, y0, abs(y1-y0)+1, color); return; }

    // Bresenham's line algorithm
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)   { int16_t t; t=x0;x0=y0;y0=t; t=x1;x1=y1;y1=t; }
    if (x0 > x1) { int16_t t; t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }

    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;

    for (; x0 <= x1; x0++) {
        if (steep) drawPixel(y0, x0, color);
        else       drawPixel(x0, y0, color);
        err -= dy;
        if (err < 0) { y0 += ystep; err += dx; }
    }
}


// ============================================================
// Circles
// ============================================================

// Internal helper: draw specific octants of a circle
void ST7796S::drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
                                uint8_t cornermask, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
        x++; ddF_x += 2; f += ddF_x;

        if (cornermask & 0x04) { drawPixel(x0+x, y0+y, color); drawPixel(x0+y, y0+x, color); }
        if (cornermask & 0x02) { drawPixel(x0+x, y0-y, color); drawPixel(x0+y, y0-x, color); }
        if (cornermask & 0x08) { drawPixel(x0-y, y0+x, color); drawPixel(x0-x, y0+y, color); }
        if (cornermask & 0x01) { drawPixel(x0-y, y0-x, color); drawPixel(x0-x, y0-y, color); }
    }
}

void ST7796S::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (r <= 0) return;
    drawPixel(x0,     y0 + r, color);
    drawPixel(x0,     y0 - r, color);
    drawPixel(x0 + r, y0,     color);
    drawPixel(x0 - r, y0,     color);
    drawCircleHelper(x0, y0, r, 0x0F, color);
}

// Internal helper: fill halves of circle (used by fillCircle and fillRoundRect)
void ST7796S::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
                                uint8_t sides, int16_t delta, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
        x++; ddF_x += 2; f += ddF_x;

        if (sides & 0x01) { // right half
            drawVLine(x0 + x, y0 - y, 2*y + delta, color);
            drawVLine(x0 + y, y0 - x, 2*x + delta, color);
        }
        if (sides & 0x02) { // left half
            drawVLine(x0 - x, y0 - y, 2*y + delta, color);
            drawVLine(x0 - y, y0 - x, 2*x + delta, color);
        }
    }
}

void ST7796S::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (r <= 0) return;
    drawVLine(x0, y0 - r, 2*r + 1, color);
    fillCircleHelper(x0, y0, r, 0x03, 1, color);
}


// ============================================================
// Rounded Rectangles
// ============================================================

void ST7796S::drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             uint16_t r, uint16_t color) {
    if (r > w/2) r = w/2;
    if (r > h/2) r = h/2;
    // Straight edges
    drawHLine(x + r,     y,         w - 2*r, color); // top
    drawHLine(x + r,     y + h - 1, w - 2*r, color); // bottom
    drawVLine(x,         y + r,     h - 2*r, color); // left
    drawVLine(x + w - 1, y + r,     h - 2*r, color); // right
    // Corners
    drawCircleHelper(x + r,         y + r,         r, 0x01, color); // top-left
    drawCircleHelper(x + w - r - 1, y + r,         r, 0x02, color); // top-right
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 0x04, color); // bottom-right
    drawCircleHelper(x + r,         y + h - r - 1, r, 0x08, color); // bottom-left
}

void ST7796S::fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             uint16_t r, uint16_t color) {
    if (r > w/2) r = w/2;
    if (r > h/2) r = h/2;
    fillRect(x + r, y, w - 2*r, h, color);
    fillCircleHelper(x + r,         y + r, r, 0x02, h - 2*r - 1, color);
    fillCircleHelper(x + w - r - 1, y + r, r, 0x01, h - 2*r - 1, color);
}


// ============================================================
// Triangles
// ============================================================

void ST7796S::drawTriangle(int16_t x0, int16_t y0,
                            int16_t x1, int16_t y1,
                            int16_t x2, int16_t y2,
                            uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void ST7796S::fillTriangle(int16_t x0, int16_t y0,
                            int16_t x1, int16_t y1,
                            int16_t x2, int16_t y2,
                            uint16_t color) {
    // Sort vertices by Y (y0 <= y1 <= y2)
    if (y0 > y1) { int16_t t; t=y0;y0=y1;y1=t; t=x0;x0=x1;x1=t; }
    if (y1 > y2) { int16_t t; t=y1;y1=y2;y2=t; t=x1;x1=x2;x2=t; }
    if (y0 > y1) { int16_t t; t=y0;y0=y1;y1=t; t=x0;x0=x1;x1=t; }

    if (y0 == y2) { // Degenerate — all on same line
        int16_t a = x0, b = x0;
        if (x1 < a) a = x1; else if (x1 > b) b = x1;
        if (x2 < a) a = x2; else if (x2 > b) b = x2;
        drawHLine(a, y0, b - a + 1, color);
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0;
    int16_t dx02 = x2 - x0, dy02 = y2 - y0;
    int16_t dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    // Upper half: y0 to y1-1 (or y1 if flat bottom)
    // Guard: only divide if dy01 != 0 (non-flat top)
    int16_t last = (y1 == y2) ? y1 : y1 - 1;
    for (int16_t y = y0; y <= last; y++) {
        int16_t a = (dy01 != 0) ? (x0 + sa / dy01) : x1;
        int16_t b = (dy02 != 0) ? (x0 + sb / dy02) : x2;
        sa += dx01; sb += dx02;
        if (a > b) { int16_t t=a; a=b; b=t; }
        drawHLine(a, y, b - a + 1, color);
    }

    // Lower half: y1 to y2
    // sa resets to 0 (starting from x1), sb continues from upper half
    // Guard: only divide if dy12 != 0 (non-flat bottom)
    sa = 0;
    sb = (int32_t)dx02 * (y1 - y0);
    for (int16_t y = y1; y <= y2; y++) {
        int16_t a = (dy12 != 0) ? (x1 + sa / dy12) : x2;
        int16_t b = (dy02 != 0) ? (x0 + sb / dy02) : x2;
        sa += dx12; sb += dx02;
        if (a > b) { int16_t t=a; a=b; b=t; }
        drawHLine(a, y, b - a + 1, color);
    }
}


// ============================================================
// Text
// ============================================================

void ST7796S::setCursor(uint16_t x, uint16_t y) {
    _cursorX = x;
    _cursorY = y;
}

void ST7796S::setTextColor(uint16_t color) {
    _textColor = color;
    _useTextBg = false;
}

void ST7796S::setTextColor(uint16_t color, uint16_t bgColor) {
    _textColor   = color;
    _textBgColor = bgColor;
    _useTextBg   = true;
}

void ST7796S::setTextSize(uint8_t size) {
    _textSize = (size < 1) ? 1 : size;
}

void ST7796S::drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t size) {
    if (c < 32 || c > 126) return;

    // Fill character cell background if bg color mode is active.
    // Cell is 6 wide (5px glyph + 1px spacing) × 8 tall (7px glyph + 1px spacing).
    if (_useTextBg) {
        fillRect(x, y, 6 * size, 8 * size, _textBgColor);
    }

    const uint8_t* glyph = FONT5X7[c - 32];
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t colData = glyph[col];
        for (uint8_t row = 0; row < 7; row++) {
            if (colData & (1 << row)) {
                if (size == 1) {
                    drawPixel(x + col, y + row, color);
                } else {
                    fillRect(x + col * size, y + row * size, size, size, color);
                }
            }
        }
    }
}

void ST7796S::print(const char* str) {
    if (!str) return;
    while (*str) {
        if (*str == '\n') {
            _cursorY += 8 * _textSize;
            _cursorX  = 0;
        } else if (*str == '\r') {
            _cursorX = 0;
        } else {
            // Soft word-wrap at right edge
            if (_cursorX + 6 * _textSize > _width) {
                _cursorY += 8 * _textSize;
                _cursorX  = 0;
            }
            drawChar(_cursorX, _cursorY, *str, _textColor, _textSize);
            _cursorX += 6 * _textSize;
        }
        str++;
    }
}

void ST7796S::println(const char* str) {
    print(str);
    _cursorY += 8 * _textSize;
    _cursorX  = 0;
}

void ST7796S::print(int value) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", value);
    print(buf);
}

void ST7796S::print(float value, uint8_t decimals) {
    char buf[24];

    if (decimals == 0) {
        snprintf(buf, sizeof(buf), "%d", (int)roundf(value));
    } else {
        bool  negative = (value < 0.0f);
        float absVal   = negative ? -value : value;
        int   whole    = (int)absVal;

        int fracMult = 1;
        for (uint8_t i = 0; i < decimals; i++) fracMult *= 10;

        int frac = (int)((absVal - (float)whole) * fracMult + 0.5f);
        if (frac >= fracMult) { whole++; frac = 0; } // carry

        if (decimals == 1) {
            snprintf(buf, sizeof(buf), "%s%d.%01d", negative ? "-" : "", whole, frac);
        } else {
            snprintf(buf, sizeof(buf), "%s%d.%02d", negative ? "-" : "", whole, frac);
        }
    }
    print(buf);
}

uint16_t ST7796S::textWidth(const char* str) {
    if (!str) return 0;
    uint16_t len = 0;
    while (*str++) len++;
    return len * 6 * _textSize;
}

uint16_t ST7796S::textHeight() {
    return 8 * _textSize;
}


// ============================================================
// Arc Gauge
// ============================================================

void ST7796S::drawGauge(uint16_t cx, uint16_t cy, uint16_t r,
                         float minVal, float maxVal, float value,
                         uint16_t needleColor) {
    // Clamp value to valid range
    if (value < minVal) value = minVal;
    if (value > maxVal) value = maxVal;

    float pct      = (value - minVal) / (maxVal - minVal);
    float endAngle = 210.0f - pct * 240.0f; // 210° (lower-left) → -30° (lower-right)

    // Arc color based on percentage of range
    uint16_t arcColor;
    if      (pct < 0.60f) arcColor = ST7796_GREEN;
    else if (pct < 0.85f) arcColor = ST7796_YELLOW;
    else                  arcColor = ST7796_RED;

    // Draw background arc (full 240° sweep in dark grey)
    for (float angle = 210.0f; angle >= -30.0f; angle -= 1.0f) {
        float   rad = angle * (float)M_PI / 180.0f;
        int16_t ax  = cx + (int16_t)((float)r * cosf(rad));
        int16_t ay  = cy - (int16_t)((float)r * sinf(rad));
        fillRect(ax - 2, ay - 2, 4, 4, ST7796_DARKGREY);
    }

    // Draw filled arc up to current value
    for (float angle = 210.0f; angle >= endAngle; angle -= 1.0f) {
        float   rad = angle * (float)M_PI / 180.0f;
        int16_t ax  = cx + (int16_t)((float)r * cosf(rad));
        int16_t ay  = cy - (int16_t)((float)r * sinf(rad));
        fillRect(ax - 2, ay - 2, 4, 4, arcColor);
    }

    // Draw needle from center hub to arc
    float   needleRad = endAngle * (float)M_PI / 180.0f;
    int16_t nx = cx + (int16_t)((float)(r - 12) * cosf(needleRad));
    int16_t ny = cy - (int16_t)((float)(r - 12) * sinf(needleRad));
    drawLine(cx, cy, nx, ny, needleColor);
    fillCircle(cx, cy, 5, needleColor); // center hub
}


// ============================================================
// Utility
// ============================================================

uint16_t ST7796S::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint16_t)(r & 0xF8) << 8) |
           ((uint16_t)(g & 0xFC) << 3) |
           ((uint16_t)(b)        >> 3);
}

uint16_t ST7796S::blendColor(uint16_t c1, uint16_t c2, uint8_t ratio) {
    // Unpack RGB565 components
    uint8_t r1 = (c1 >> 11) & 0x1F;
    uint8_t g1 = (c1 >>  5) & 0x3F;
    uint8_t b1 = (c1)       & 0x1F;

    uint8_t r2 = (c2 >> 11) & 0x1F;
    uint8_t g2 = (c2 >>  5) & 0x3F;
    uint8_t b2 = (c2)       & 0x1F;

    // Linear interpolation
    uint8_t r = r1 + ((int16_t)(r2 - r1) * ratio) / 255;
    uint8_t g = g1 + ((int16_t)(g2 - g1) * ratio) / 255;
    uint8_t b = b1 + ((int16_t)(b2 - b1) * ratio) / 255;

    return ((uint16_t)r << 11) | ((uint16_t)g << 5) | b;
}
