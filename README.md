# ST7796S — TFT Display Driver for Particle Photon 2

A lightweight, self-contained driver for the **ST7796S 480×320 TFT display** over hardware SPI, written natively for the **Particle Photon 2**. No Adafruit dependencies. No Adafruit_GFX. Just the Particle firmware and this library.

> **Why does this exist?**  
> No ST7796S library existed for Particle devices. The closest alternative was using `Adafruit_ILI9341` as a partial stand-in, which has initialization differences, missing features, and requires the full Adafruit_GFX stack. This library drives the ST7796S correctly from scratch.

---

## Tested Hardware

| Component | Part |
|-----------|------|
| Microcontroller | Particle Photon 2 |
| Display module | 4.0" TFT SPI 480×320 V1.0 (MSP4031) |
| Display driver IC | ST7796S |
| Touch controller | FT6336U (capacitive, I²C) — see [FT6336U driver](#touch-support) |

---

## Wiring (Particle Photon 2)

| Display Pin | Photon 2 Pin | Notes |
|-------------|--------------|-------|
| VCC | 3V3 or VUSB (5V) | 5V gives brighter backlight |
| GND | GND | |
| LCD_CS | D5 | Configurable in constructor |
| LCD_RST | D6 | Configurable in constructor |
| LCD_RS (DC) | D7 | Configurable in constructor |
| SDI (MOSI) | S0 (MO) | Hardware SPI — fixed |
| SCK | S2 (SCK) | Hardware SPI — fixed |
| SDO (MISO) | S1 (MI) | Optional — only needed for read-back |
| LED (backlight) | 3V3 | Connect for always-on; use GPIO + PWM for brightness control |

> **Note on backlight:** Many MSP4031 modules have the backlight pull-up hardwired on the PCB. If you disconnect the LED pin and the backlight stays on, your module is one of these. The LED pin is only needed if you want software brightness control.

---

## Installation

### Particle Workbench (VS Code)
1. Copy the `src/ST7796S.h` and `src/ST7796S.cpp` files into your project's `lib/ST7796S/src/` folder.
2. Add `#include "ST7796S.h"` to your application file.

### Particle CLI
```bash
particle library add ST7796S
```
*(once published to the Particle library registry)*

---

## Quick Start

```cpp
#include "Particle.h"
#include "ST7796S.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

// CS=D5, DC=D7, RST=D6
ST7796S tft(D5, D7, D6);

void setup() {
    tft.begin();
    tft.setRotation(0);         // 0=landscape 480x320, 1=portrait 320x480
    tft.fillScreen(ST7796_BLACK);

    tft.setTextColor(ST7796_WHITE, ST7796_BLACK);
    tft.setTextSize(3);
    tft.setCursor(80, 140);
    tft.print("Hello Photon 2!");
}

void loop() {}
```

---

## API Reference

### Initialization

```cpp
ST7796S tft(csPin, dcPin, rstPin);  // Constructor
tft.begin();                         // Initialize — call once in setup()
tft.setRotation(r);                  // 0–3, see rotation table below
```

**Rotation values:**

| Value | Orientation | Width | Height |
|-------|-------------|-------|--------|
| 0 | Landscape (default) | 480 | 320 |
| 1 | Portrait (rotated 90° CW) | 320 | 480 |
| 2 | Landscape (flipped 180°) | 480 | 320 |
| 3 | Portrait (rotated 90° CCW) | 320 | 480 |

---

### Drawing

```cpp
// Screen
tft.fillScreen(color);
tft.width();   // current width  (swaps with rotation)
tft.height();  // current height (swaps with rotation)

// Rectangles
tft.fillRect(x, y, w, h, color);
tft.drawRect(x, y, w, h, color);
tft.drawBorder(x, y, w, h, color, thickness);
tft.fillRoundRect(x, y, w, h, radius, color);
tft.drawRoundRect(x, y, w, h, radius, color);

// Pixels & Lines
tft.drawPixel(x, y, color);
tft.drawLine(x0, y0, x1, y1, color);
tft.drawHLine(x, y, w, color);   // fast horizontal line
tft.drawVLine(x, y, h, color);   // fast vertical line

// Circles
tft.drawCircle(cx, cy, r, color);
tft.fillCircle(cx, cy, r, color);

// Triangles
tft.drawTriangle(x0,y0, x1,y1, x2,y2, color);
tft.fillTriangle(x0,y0, x1,y1, x2,y2, color);
```

---

### Text

```cpp
tft.setCursor(x, y);
tft.setTextSize(size);               // 1=5x7px native, 2=10x14px, etc.
tft.setTextColor(fg);                // transparent background
tft.setTextColor(fg, bg);            // solid background — use for dynamic text
tft.print("Hello");                  // string
tft.print(42);                       // integer
tft.print(3.14f, 2);                 // float, 2 decimal places
tft.println("Line with newline");

// Helpers for layout
uint16_t w = tft.textWidth("Hello"); // pixel width of string
uint16_t h = tft.textHeight();       // pixel height at current size
```

> **Tip:** Always use `setTextColor(fg, bg)` when printing values that change over time. The background fill prevents old digit ghosting without needing a manual `fillRect()` clear.

---

### Colors

Pre-defined RGB565 constants:

```cpp
ST7796_BLACK     ST7796_WHITE     ST7796_RED       ST7796_GREEN
ST7796_BLUE      ST7796_YELLOW    ST7796_CYAN      ST7796_MAGENTA
ST7796_ORANGE    ST7796_DARKGREY  ST7796_LIGHTGREY ST7796_NAVY
ST7796_DARKGREEN ST7796_MAROON    ST7796_PURPLE    ST7796_OLIVE
ST7796_TEAL      ST7796_PINK
```

Custom colors:
```cpp
uint16_t myColor = ST7796S::color565(255, 128, 0);        // orange from RGB
uint16_t blended = ST7796S::blendColor(ST7796_RED, ST7796_BLUE, 128); // 50/50 blend
```

---

### Gauge (Dashboard Helper)

```cpp
// 240° arc gauge — automotive speedometer style
// Color: green (0–60%) → yellow (60–85%) → red (85–100%)
tft.drawGauge(
    cx, cy,          // center coordinates
    r,               // arc radius in pixels
    0.0f, 100.0f,    // min and max values
    currentValue,    // current value (clamped automatically)
    ST7796_WHITE     // needle and hub color
);
```

---

## Performance Notes

- **fillRect()** is the workhorse — all filled shapes use it internally. Speed scales with pixel count.
- **fillScreen()** takes ~300ms at 20 MHz SPI. Avoid calling it in your main loop.
- **Partial updates** are far faster than full redraws. Clear only the region you're updating.
- **Avoid redrawing unchanged content.** Track previous values and only call draw functions when data changes — this eliminates visible flicker entirely.
- SPI clock is set to 20 MHz by default (`ST7796_SPI_MHZ`). Reduce to 10 MHz in `ST7796S.h` if you see display corruption.

---

## Touch Support

This library handles display only. For the FT6336U capacitive touch controller found on the MSP4031 module, see the companion driver in this repo:

**[FT6336U.h / FT6336U.cpp](src/FT6336U.h)**

Wiring for touch (I²C, independent of display SPI):

| Display Pin | Photon 2 Pin |
|-------------|--------------|
| CTP_SCL | D1 |
| CTP_SDA | D0 |
| CTP_RST | D2 |
| CTP_INT | D3 |

---

## Known Issues & Notes

- **Color inversion:** The ST7796S command `0x21` (Display Inversion ON) is intentionally not sent during initialization. Sending it causes all colors to display as their bitwise complement (red appears cyan, green appears magenta, etc.). If you use this library with a different module and colors appear inverted, check whether your module requires `0x21`.
- **Rotation MADCTL values** are validated for the MSP4031 module. Different ST7796S breakout boards may use different MADCTL values depending on how the panel is physically mounted.

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Credits

Developed by **estyntech** for the **NC State SolarPack** solar car team (Fenrir vehicle, SPIN Board display system).

Initialization sequence derived from the official ST7796S datasheet and the [lcdwiki.com MSP4031 module documentation](https://www.lcdwiki.com/4.0inch_Capacitive_SPI_Module_ST7796).
