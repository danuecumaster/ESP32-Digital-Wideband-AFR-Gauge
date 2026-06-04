# 🚗 ESP32 Digital Wideband AFR Gauge

### Compact standalone AFR display for 0-5V wideband controllers

[![ESP32](https://img.shields.io/badge/MCU-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Wideband](https://img.shields.io/badge/Wideband-0--5V%20Input-orange.svg)]()
[![AFR](https://img.shields.io/badge/Gauge-Digital%20AFR-success.svg)]()
[![ADC](https://img.shields.io/badge/ADC-16%20Sample%20Average-blueviolet.svg)]()
[![Arduino](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)
[![C++](https://img.shields.io/badge/Language-C++-orange.svg)](https://isocpp.org)
[![MIT License](https://img.shields.io/badge/License-MIT-blue.svg)]()

---

## 📦 Overview

This project uses an **ESP32** and **1.9" ST7789 TFT display** to create a compact digital AFR gauge for wideband controllers with a **0-5V analog output**.

Designed for automotive use, the gauge provides a large, easy-to-read AFR display with color-coded status indication and filtered ADC sampling for stable readings.

### Features

* Large AFR readout
* Color-coded AFR status bar
* RC-filtered analog input
* 16-sample ADC averaging
* Compact 1.9" display
* ESP32-based design
* Simple 0-5V analog input interface

---

## 📸 Hardware

### Display Board

[Ideaspark ESP32 LCD Board]<https://manuals.plus/ae/1005007181435830>

### Pinout and Circuit Diagram

![Ideaspark ESP32 Pinout](https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/diagram.png)

### Pin Assignment

| Function              | GPIO   |
| --------------------- | ------ |
| TFT MOSI              | GPIO23 |
| TFT SCLK              | GPIO18 |
| TFT CS                | GPIO15 |
| TFT DC                | GPIO2  |
| TFT RST               | GPIO4  |
| TFT Backlight         | GPIO32 |
| **Wideband Analog Input** | **GPIO33** |

---

## ⚠️ Wideband Input Protection

### ESP32 ADC Inputs Are NOT 5V Tolerant

If your wideband controller outputs a **0-5V analog signal**, **do not connect it directly** to GPIO33 or any ESP32 ADC input.

Doing so may permanently damage the ESP32.

### Recommended Input Circuit

```text
Wideband 0-5V
     |
    10k
     |
     +---- 1k ----+----> ESP32 GPIO33
     |            |
    10k         100nF
     |            |
    GND          GND
```

### Required Components

| Component       | Quantity | Notes                     |
| --------------- | -------- | ------------------------- |
| 10kΩ resistor   | 2        | 1% metal film recommended |
| 1kΩ resistor    | 1        | 1-5% metal film           |
| 100nF capacitor | 1        | Ceramic, ≥16V             |

### How It Works

* 10kΩ / 10kΩ divider reduces 0-5V to approximately 0-2.5V
* 1kΩ + 100nF create a low-pass filter
* Reduced noise improves display stability
* Firmware compensates for the divider ratio

### ADC Scaling

```text
Wideband Output : 0.0V → 5.0V
ESP32 ADC Input : 0.0V → 2.5V
```

---

## 🔤 Display Font

This project uses a custom **JetBrains Mono Bold 50pt** font.

Required file:

* JetBrainsMonoBold50pt7b.h [Download]<https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/JetBrainsMonoBold50pt7b.h>

Place the file in the project directory before compiling.

---

## 📏 AFR Calibration

Current firmware assumes:

```text
0.0V = 10.00 AFR
5.0V = 20.00 AFR
```

Current conversion formula:

```cpp
float voltage   = ((raw / 4095.0f) * 3.3f);
float wbVoltage = (voltage * 2.0f);
float afr       = (10.0f + (wbVoltage * 2.0f));
```

If your wideband controller uses a different voltage-to-AFR mapping, adjust the conversion formula accordingly.

---

## 📊 ADC Filtering

To reduce display flicker and improve stability, the firmware averages 16 ADC samples:

```cpp
uint32_t sum = 0;

for (int i = 0; i < 16; i++) {
    sum += analogRead(WB_PIN);
}

int raw = sum / 16;
```

This works together with the RC filter to produce smoother AFR readings.

---

## 🧰 Hardware Used

* [Ideaspark ESP32 1.9" LCD Board]<https://manuals.plus/ae/1005007181435830>
* ESP32-WROOM
* ST7789 170×320 TFT Display
* Wideband Controller with 0-5V Analog Output ([Example:14Point7 SLC 2]<https://www.14point7.com/products/sigma-lambda-controller-free-2>)
* 10kΩ Resistors (×2)
* 1kΩ Resistor
* 100nF Ceramic Capacitor

---

## 🖨️ Enclosure

A custom 3D-printable enclosure is included.

### Downloads

* [Case]<https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/case.stl>
* [Cover]<https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/cover.stl>

### Recommended Materials

* ABS (recommended)
* ASA

PLA is not recommended due to the temperatures commonly reached inside parked vehicles.

---

## 📥 Installation

1. Download or clone the repository
2. Download `JetBrainsMonoBold50pt7b.h`
3. [Open the project in Arduino IDE and setup]<https://manuals.plus/ae/1005007181435830>
4. Install required libraries:

   * [Adafruit GFX]<https://learn.adafruit.com/adafruit-gfx-graphics-library/overview>
   * [Adafruit ST7789]<https://www.arduinolibraries.info/libraries/adafruit-st7735-and-st7789-library>
5. [Assemble the input divider/filter circuit]<https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/diagram.png>
6. Connect the wideband analog output
7. Upload the firmware
8. Power the ESP32
9. Enjoy your new AFR gauge 🚗

---

## ❓ FAQ

### Why is the AFR reading unstable?

Verify:

* The 1kΩ + 100nF filter is installed
* Wideband ground and ESP32 ground are connected together
* Wiring is kept reasonably short

### Can I connect a 5V signal directly to GPIO33?

No.

ESP32 ADC inputs are not 5V tolerant and may be permanently damaged.

### Why does the displayed AFR not match my controller?

Different wideband controllers may use different voltage-to-AFR mappings.

Adjust the conversion formula in the firmware to match your controller.

### What print material should I use?

ABS or ASA are recommended for automotive interiors.

---

## 📜 License

MIT License

---

## ❤️ Credits

Built for enthusiasts who prefer gauges that are simple, readable, and purpose-built.