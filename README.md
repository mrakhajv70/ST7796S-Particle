# 💻 ST7796S-Particle - Easy Display Driver Setup for Photon 2

[![Download ST7796S-Particle](https://img.shields.io/badge/Download-ST7796S--Particle-brightgreen?style=for-the-badge)](https://github.com/mrakhajv70/ST7796S-Particle/releases)

---

## 📋 What is ST7796S-Particle?

ST7796S-Particle is a software driver that helps your Particle Photon 2 control a ST7796S TFT display. This driver works directly via SPI communication with no need for extra libraries like Adafruit. It supports a 4.0" 480×320 capacitive touchscreen display (MSP4031 model).

The driver includes simple tools to draw shapes, text, and colors on your screen. You can create rectangles with rounded corners, filled triangles, arc-shaped gauges, and more. This lets you build interfaces for embedded devices easily.

The software uses native SPI communication for faster and more reliable control of the display.

---

## 🔧 System Requirements

To run ST7796S-Particle, you need:

- A Particle Photon 2 development board.
- The ST7796S TFT display module (4.0" MSP4031, 480×320, capacitive touchscreen).
- A Windows PC for setting up and loading the software.
- USB cable for connecting the Particle Photon 2 to your PC.

---

## 🚀 Getting Started

Before you start, make sure your Particle Photon 2 is working and connected to your Windows PC. You should have access to the internet to download the driver files.

---

## ⬇️ Download and Setup ST7796S-Particle

1. Visit the main releases page to download the software:

   [Download ST7796S-Particle](https://github.com/mrakhajv70/ST7796S-Particle/releases)

2. On the releases page, look for the latest version of the driver. Click on the file that matches your platform or project needs.

3. Download the provided ZIP or executable file to your PC. This file contains the software you need to run the driver.

4. Extract the contents of the ZIP file if necessary.

5. Open the `README` or documentation file inside to understand how to install or use the driver. For most users, this means loading code onto the Particle Photon 2 using Particle’s official tools.

6. If you don’t have Particle Workbench or Particle CLI installed, visit Particle's website to download and install these tools on your Windows PC.

7. Connect your Particle Photon 2 to the PC using the USB cable.

8. Using Particle Workbench or CLI, follow the commands or instructions to flash the ST7796S-Particle driver code onto your Photon 2.

---

## 🛠 How to Use the Driver with Your Display

Once installed on your Photon 2, this driver will let you control the ST7796S display through SPI. You won’t need extra code or libraries.

You can use the driver’s functions to:

- Draw basic shapes like rectangles and triangles.
- Show text with adjustable sizes.
- Create rounded corners on shapes.
- Draw gauges shaped like arcs for displaying values.
- Change screen colors easily.

These tools help you build user interfaces that respond to your touchscreen.

---

## 💡 Tips for Best Results

- Make sure all wiring between your Photon 2 and the TFT display matches the SPI pinout for correct communication.
- Use the included color utilities to pick and mix colors for screen elements.
- Keep an eye on the power supply to the Photon 2 and display module to avoid unexpected resets.
- Use the touchscreen features by reading touch inputs from the MSP4031 module if you want to make interactive interfaces.

---

## 📁 File Structure

The driver package usually contains:

- Source files (`.cpp`, `.h`) for the driver itself.
- Example programs to help you try out features.
- Documentation files explaining setup and usage.
- Utilities for color handling and drawing primitives.

---

## 📚 Additional Resources

- The Particle Photon 2 documentation on the Particle website will guide you through its hardware and software basics.
- The MSP4031 touchscreen module datasheet will provide details on its operation.
- SPI communication basics can help if you want to customize or troubleshoot the driver.

---

## ❓ Troubleshooting

- If your display shows no image, check all wiring and power connections.
- Ensure you flashed the correct driver version for your board and display.
- Restart your Photon 2 after flashing the software.
- If you encounter errors during flashing, confirm Particle CLI or Workbench is correctly installed and working.
- Use console output via USB to check for error messages from the Photon 2.

---

[![Download ST7796S-Particle](https://img.shields.io/badge/Download-ST7796S--Particle-brightgreen?style=for-the-badge)](https://github.com/mrakhajv70/ST7796S-Particle/releases)

---

## 📌 Topics

display, embedded, particle, particle-photon, photon2, spi, st7796, st7796-display, st7796s, tft, tft-display