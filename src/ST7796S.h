/**
 * @file ST7796S.h
 * @brief ST7796S TFT Display Driver for Particle Photon 2
 *
 * A lightweight, self-contained driver for the ST7796S 480x320 TFT display
 * over hardware SPI. No external dependencies beyond the Particle firmware.
 *
 * Tested hardware:
 *   - Particle Photon 2
 *   - 4.0" TFT SPI 480x320 V1.0 (MSP4031, ST7796S driver IC)
 *
 * Default wiring (Particle Photon 2):
 *   Display VCC  → 3V3 (or 5V for brighter backlight)
 *   Display GND  → GND
 *   Display CS   → D5       (or any GPIO, passed to constructor)
 *   Display RST  → D6       (or any GPIO, passed to constructor)
 *   Display DC   → D7       (or any GPIO, passed to constructor)
 *   Display MOSI → S0 (MO)
 *   Display SCK  → S2 (SCK)
 *   Display MISO → S1 (MI)  (optional — only needed if reading from display)
 *   Display LED  → 3V3      (backlight always-on) or GPIO for PWM control
 *
 * Usage:
 * @code
 *   #include "ST7796S.h"
 *   ST7796S tft(D5, D7, D6); // CS, DC, RST
 *
 *   void setup() {
 *       tft.begin();
 *       tft.setRotation(0);
 *       tft.fillScreen(ST7796_BLACK);
 *       tft.setTextColor(ST7796_WHITE);
 *       tft.setTextSize(2);
 *       tft.setCursor(10, 10);
 *       tft.print("Hello Photon 2!");
 *   }
 * @endcode
 *
 * @author NC State SolarPack — Estyn Stevenson-Gentry
 * @version 1.0.0
 * @date 2026
 * @license MIT
 *
 */

#pragma once
#include "Particle.h"

// ============================================================
// Library Version
// ============================================================
#define ST7796S_LIB_VERSION "1.0.0"

// ============================================================
// Color Definitions (RGB565)
// Use color565(r, g, b) to create custom colors at runtime.
// ============================================================
#define ST7796_BLACK      0x0000
#define ST7796_WHITE      0xFFFF
#define ST7796_RED        0xF800
#define ST7796_GREEN      0x07E0
#define ST7796_BLUE       0x001F
#define ST7796_YELLOW     0xFFE0
#define ST7796_CYAN       0x07FF
#define ST7796_MAGENTA    0xF81F
#define ST7796_ORANGE     0xFD20
#define ST7796_DARKGREY   0x7BCF
#define ST7796_LIGHTGREY  0xCE59
#define ST7796_NAVY       0x000F
#define ST7796_DARKGREEN  0x03E0
#define ST7796_MAROON     0x7800
#define ST7796_PURPLE     0x780F
#define ST7796_OLIVE      0x7BE0
#define ST7796_TEAL       0x03EF
#define ST7796_PINK       0xF81F

// ============================================================
// Display Native Dimensions
// These reflect the physical pixel count of the ST7796S panel.
// width() and height() will swap automatically with setRotation().
// ============================================================
#define ST7796_NATIVE_WIDTH  480
#define ST7796_NATIVE_HEIGHT 320

// ============================================================
// SPI Clock Speed
// 20 MHz is the maximum reliable speed for most ST7796S modules
// on Photon 2 hardware SPI. Reduce to 10 MHz if you see
// display corruption or SPI errors.
// ============================================================
#define ST7796_SPI_MHZ 20


/**
 * @class ST7796S
 * @brief Driver class for the ST7796S TFT display.
 *
 * Instantiate once globally, call begin() in setup(), then use
 * the drawing methods freely in loop() or any function.
 */
class ST7796S {
public:

    // --------------------------------------------------------
    // Construction & Initialization
    // --------------------------------------------------------

    /**
     * @brief Constructor. Stores pin assignments — does not touch hardware yet.
     * @param csPin   GPIO connected to display CS  (chip select, active LOW)
     * @param dcPin   GPIO connected to display DC  (data/command select)
     * @param rstPin  GPIO connected to display RST (reset, active LOW)
     */
    ST7796S(uint8_t csPin, uint8_t dcPin, uint8_t rstPin);

    /**
     * @brief Initialize the display. Must be called once in setup().
     *
     * Configures GPIO pins, starts hardware SPI at ST7796_SPI_MHZ,
     * performs a hardware reset, and sends the full ST7796S
     * initialization sequence. Display is ready to draw after this returns.
     */
    void begin();

    /**
     * @brief Set display rotation.
     * @param r  0 = landscape (480x320, default)
     *           1 = portrait  (320x480, rotated 90° CW)
     *           2 = landscape (480x320, flipped 180°)
     *           3 = portrait  (320x480, rotated 90° CCW)
     *
     * Updates width() and height() to reflect the new orientation.
     * Call after begin() and before drawing anything, or any time
     * you need to change orientation at runtime.
     */
    void setRotation(uint8_t r);


    // --------------------------------------------------------
    // Screen & Rectangle
    // --------------------------------------------------------

    /**
     * @brief Fill the entire screen with one color.
     * @param color  RGB565 color value
     */
    void fillScreen(uint16_t color);

    /**
     * @brief Fill a rectangle with a solid color.
     * @param x      Top-left X coordinate
     * @param y      Top-left Y coordinate
     * @param w      Width in pixels
     * @param h      Height in pixels
     * @param color  RGB565 color value
     */
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

    /**
     * @brief Draw a 1-pixel-wide rectangle outline.
     * @param x      Top-left X coordinate
     * @param y      Top-left Y coordinate
     * @param w      Width in pixels
     * @param h      Height in pixels
     * @param color  RGB565 color value
     */
    void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

    /**
     * @brief Draw a rectangle with a thick border.
     * @param x          Top-left X coordinate
     * @param y          Top-left Y coordinate
     * @param w          Width in pixels
     * @param h          Height in pixels
     * @param color      RGB565 color value
     * @param thickness  Border thickness in pixels (1 = same as drawRect)
     */
    void drawBorder(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    uint16_t color, uint8_t thickness);

    /**
     * @brief Draw a rounded rectangle outline.
     * @param x      Top-left X coordinate
     * @param y      Top-left Y coordinate
     * @param w      Width in pixels
     * @param h      Height in pixels
     * @param r      Corner radius in pixels
     * @param color  RGB565 color value
     */
    void drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                       uint16_t r, uint16_t color);

    /**
     * @brief Draw a filled rounded rectangle.
     * @param x      Top-left X coordinate
     * @param y      Top-left Y coordinate
     * @param w      Width in pixels
     * @param h      Height in pixels
     * @param r      Corner radius in pixels
     * @param color  RGB565 color value
     */
    void fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                       uint16_t r, uint16_t color);


    // --------------------------------------------------------
    // Pixels & Lines
    // --------------------------------------------------------

    /**
     * @brief Draw a single pixel.
     * @param x      X coordinate
     * @param y      Y coordinate
     * @param color  RGB565 color value
     */
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Draw a line between two points (Bresenham's algorithm).
     * @param x0     Start X
     * @param y0     Start Y
     * @param x1     End X
     * @param y1     End Y
     * @param color  RGB565 color value
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

    /**
     * @brief Draw a horizontal line. Faster than drawLine() for horizontal runs.
     * @param x      Start X
     * @param y      Y coordinate
     * @param w      Width (number of pixels)
     * @param color  RGB565 color value
     */
    void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);

    /**
     * @brief Draw a vertical line. Faster than drawLine() for vertical runs.
     * @param x      X coordinate
     * @param y      Start Y
     * @param h      Height (number of pixels)
     * @param color  RGB565 color value
     */
    void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);


    // --------------------------------------------------------
    // Circles & Triangles
    // --------------------------------------------------------

    /**
     * @brief Draw a circle outline.
     * @param x0     Center X
     * @param y0     Center Y
     * @param r      Radius in pixels
     * @param color  RGB565 color value
     */
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

    /**
     * @brief Draw a filled circle.
     * @param x0     Center X
     * @param y0     Center Y
     * @param r      Radius in pixels
     * @param color  RGB565 color value
     */
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

    /**
     * @brief Draw a triangle outline.
     * @param x0,y0  First vertex
     * @param x1,y1  Second vertex
     * @param x2,y2  Third vertex
     * @param color  RGB565 color value
     */
    void drawTriangle(int16_t x0, int16_t y0,
                      int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2,
                      uint16_t color);

    /**
     * @brief Draw a filled triangle.
     * @param x0,y0  First vertex
     * @param x1,y1  Second vertex
     * @param x2,y2  Third vertex
     * @param color  RGB565 color value
     */
    void fillTriangle(int16_t x0, int16_t y0,
                      int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2,
                      uint16_t color);


    // --------------------------------------------------------
    // Text
    // --------------------------------------------------------

    /**
     * @brief Set the text cursor position for the next print() call.
     * @param x  X coordinate (pixels from left)
     * @param y  Y coordinate (pixels from top)
     */
    void setCursor(uint16_t x, uint16_t y);

    /**
     * @brief Set text foreground color with transparent background.
     *
     * Pixels in the character that are "off" will not be drawn,
     * leaving whatever is already on screen behind them. Use
     * setTextColor(fg, bg) instead if you need clean redraws.
     *
     * @param color  RGB565 foreground color
     */
    void setTextColor(uint16_t color);

    /**
     * @brief Set text foreground and background color.
     *
     * The full character cell (6 x 8 pixels × textSize) is filled
     * with bgColor before drawing the character. This eliminates
     * ghosting when updating text that changes frequently.
     *
     * @param color    RGB565 foreground color
     * @param bgColor  RGB565 background color
     */
    void setTextColor(uint16_t color, uint16_t bgColor);

    /**
     * @brief Set text scale factor.
     * @param size  1 = native 5×7px, 2 = 10×14px, 3 = 15×21px, etc.
     *              Minimum value is 1.
     */
    void setTextSize(uint8_t size);

    /**
     * @brief Print a null-terminated string at the current cursor position.
     *        Newline characters (\n) advance the cursor to the next line.
     * @param str  C-string to print
     */
    void print(const char* str);

    /**
     * @brief Print a signed integer at the current cursor position.
     * @param value  Integer to print
     */
    void print(int value);

    /**
     * @brief Print a float at the current cursor position.
     * @param value     Float to print
     * @param decimals  Number of decimal places (default: 1, max: 2)
     */
    void print(float value, uint8_t decimals = 1);

    /**
     * @brief Print a string followed by a newline.
     * @param str  C-string to print
     */
    void println(const char* str);

    /**
     * @brief Get the pixel width of a string at the current text size.
     *        Useful for centering text manually.
     * @param str  C-string to measure
     * @return     Width in pixels
     */
    uint16_t textWidth(const char* str);

    /**
     * @brief Get the pixel height of a character at the current text size.
     * @return  Height in pixels (8 * textSize)
     */
    uint16_t textHeight();


    // --------------------------------------------------------
    // Dashboard / Gauge Helpers
    // --------------------------------------------------------

    /**
     * @brief Draw an arc-style gauge (automotive speedometer style).
     *
     * Draws a 240-degree filled arc from 210° to -30° (clockwise from
     * lower-left to lower-right). The arc color changes from green to
     * yellow to red as the value increases through its range. A needle
     * line and center hub circle are drawn in needleColor.
     *
     * The entire arc is redrawn on each call. For smooth animation,
     * call drawGauge() with updated values in your loop — the arc
     * redraws itself cleanly without needing a manual clear.
     *
     * Color zones:
     *   0–60%  of range → ST7796_GREEN
     *   60–85% of range → ST7796_YELLOW
     *   85–100%of range → ST7796_RED
     *
     * @param cx          Center X coordinate
     * @param cy          Center Y coordinate
     * @param r           Arc radius in pixels
     * @param minVal      Minimum gauge value (needle at 210°)
     * @param maxVal      Maximum gauge value (needle at -30°)
     * @param value       Current value to display (clamped to [minVal, maxVal])
     * @param needleColor RGB565 color for needle line and center hub
     */
    void drawGauge(uint16_t cx, uint16_t cy, uint16_t r,
                   float minVal, float maxVal, float value,
                   uint16_t needleColor);


    // --------------------------------------------------------
    // Utility
    // --------------------------------------------------------

    /**
     * @brief Convert 8-bit R, G, B values to a 16-bit RGB565 color.
     * @param r  Red   (0–255)
     * @param g  Green (0–255)
     * @param b  Blue  (0–255)
     * @return   RGB565 packed color value
     */
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Blend two RGB565 colors together.
     * @param c1     First color
     * @param c2     Second color
     * @param ratio  Blend ratio 0–255 (0 = all c1, 255 = all c2)
     * @return       Blended RGB565 color
     */
    static uint16_t blendColor(uint16_t c1, uint16_t c2, uint8_t ratio);

    /**
     * @brief Current display width in pixels (accounts for rotation).
     * @return Width in pixels
     */
    uint16_t width()  { return _width; }

    /**
     * @brief Current display height in pixels (accounts for rotation).
     * @return Height in pixels
     */
    uint16_t height() { return _height; }

    /**
     * @brief Current rotation index (0–3).
     * @return Rotation value last set by setRotation()
     */
    uint8_t  rotation() { return _rotation; }

    /**
     * @brief Current cursor X position.
     * @return X coordinate in pixels
     */
    uint16_t getCursorX() { return _cursorX; }

    /**
     * @brief Current cursor Y position.
     * @return Y coordinate in pixels
     */
    uint16_t getCursorY() { return _cursorY; }


private:
    // Pin assignments
    uint8_t  _cs, _dc, _rst;

    // Display state
    uint16_t _width, _height;
    uint16_t _cursorX, _cursorY;
    uint16_t _textColor;
    uint16_t _textBgColor;
    bool     _useTextBg;
    uint8_t  _textSize;
    uint8_t  _rotation;

    // Low-level SPI transactions
    void     writeCommand(uint8_t cmd);
    void     writeData(uint8_t data);
    void     writeData16(uint16_t data);
    void     setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    // Hardware initialization sequence
    void     initST7796S();

    // Internal glyph renderer
    void     drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t size);

    // Internal circle arc helper (used by drawCircle, fillCircle, drawRoundRect)
    void     drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
                              uint8_t cornermask, uint16_t color);
    void     fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
                              uint8_t sides, int16_t delta, uint16_t color);
};
