# ESP32 Digital Wideband AFR Gauge

### Compact standalone AFR display for 0-5V wideband controllers

[![ESP32](https://img.shields.io/badge/MCU-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![ADS1115](https://img.shields.io/badge/ADC-ADS1115%2016--Bit-green.svg)]()
[![Wideband](https://img.shields.io/badge/Wideband-0--5V%20Input-orange.svg)]()
[![AFR](https://img.shields.io/badge/Gauge-Digital%20AFR-success.svg)]()														 
[![Arduino](https://img.shields.io/badge/Framework-Arduino-blue.svg)](https://www.arduino.cc/)
[![C++](https://img.shields.io/badge/Language-C++-orange.svg)](https://isocpp.org)
[![MIT License](https://img.shields.io/badge/License-MIT-blue.svg)]()

---

## 📦 Overview

This project uses an **ESP32**, **ADS1115 16-bit ADC**, and **1.9" ST7789 TFT display** to create a compact digital AFR gauge for wideband controllers with a **0-5V analog output**.

Designed for automotive use, the gauge provides a large, easy-to-read AFR display with color-coded status indication and filtered ADC sampling for stable readings.

### Features

* ESP32-based design
* ADS1115 16-bit external ADC
* Compact 1.9" display
* Large mono-space AFR readout
* Color-coded AFR status bar
* ADC calibration support
* Configurable ADC averaging
* RC-filtered 0-5V analog input interface
* Voltage divider input protection

---

## 📸 Hardware

### Display Board

[Ideaspark ESP32 LCD Board](https://manuals.plus/ae/1005007181435830)

### ADC

[ADS1115 16-bit I²C Analog-to-Digital Converter](https://www.ti.com/lit/ds/symlink/ads1115.pdf)

### Pinout and Circuit Diagram

![Ideaspark ESP32 Pinout](https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/diagram.png)

### Pin Assignment

| Function      | GPIO   |
| ------------- | ------ |
| TFT MOSI      | GPIO23 |
| TFT SCLK      | GPIO18 |
| TFT CS        | GPIO15 |
| TFT DC        | GPIO2  |
| TFT RST       | GPIO4  |
| TFT Backlight | GPIO32 |
| ADS1115 SDA   | GPIO21 |
| ADS1115 SCL   | GPIO22 |

Wideband input is connected to **ADS1115 channel A0**.

---

## ⚠️ Wideband Input Protection

### ADS1115 Inputs Are NOT 5V Tolerant

If your wideband controller outputs a **0-5V analog signal**, do **not** connect it directly to the ADS1115 input.

Doing so may damage the ADS1115 or cause inaccurate readings.

### Recommended Input Circuit

```text
Wideband 0-5V
     |
    10k
     |
     +---- 1k ----+----> ADS1115 A0
     |            |
    10k         100nF
     |            |
    GND          GND
```

### Required Components

| Component       | Quantity | Notes                     |
| --------------- | -------- | ------------------------- |
| 10kΩ resistor   | 2        | 1% metal film recommended |
| 1kΩ resistor    | 1        | 1%-5% metal film          |
| 100nF capacitor | 1        | Ceramic, ≥16V             |

### How It Works

* 10kΩ / 10kΩ divider reduces 0-5V to approximately 0-2.5V
* 1kΩ + 100nF create a low-pass filter
* ADS1115 measures the filtered signal on channel A0
* Firmware compensates for the divider ratio
* ESP32 communicates with the ADS1115 over I²C

### ADC Scaling

```text
Wideband Output : 0.0V → 5.0V
ADS1115 Input   : 0.0V → 2.5V
```

---

## 🔤 Display Font

This project uses a custom **JetBrains Mono Bold 50pt** font.

Required file:

* [JetBrainsMonoBold50pt7b.h](https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/JetBrainsMonoBold50pt7b.h)

Place the file in the project directory before compiling.

---

## 📏 AFR Calibration

Current firmware assumes:

```text
0.0V → 10.00 AFR
5.0V → 20.00 AFR
```

Current conversion formula:

```cpp
float voltage 	= (rawV * V_MULTIPLIER * ADC_CORRECTION);
float afr 		= (AFR_MIN + (voltage * WBO_MULTIPLIER));
												   
```

Where:

```text
rawV            = ADS1115 measured voltage
V_MULTIPLIER    = Divider compensation (2.0)
ADC_CORRECTION  = ADC calibration factor
WBO_MULTIPLIER  = AFR scaling factor
```

If your wideband controller uses a different voltage-to-AFR mapping, adjust the conversion formula accordingly.

---

## 🎯 ADC Calibration

The ADS1115 is generally very accurate, but resistor tolerances in the divider network may introduce small measurement errors.

For maximum accuracy, the firmware includes an optional correction factor:

```cpp
#define ADC_CORRECTION 1.0000f
```

Voltage calculation:

```cpp
float voltage = (rawV * V_MULTIPLIER * ADC_CORRECTION);
```

### Calibration Procedure

1. Apply a known voltage to the wideband input.
2. Measure the voltage using a quality multimeter.
3. Compare the measured voltage against the voltage reported by the firmware.
4. Calculate the correction factor:

```text
ADC_CORRECTION = Actual Voltage / Measured Voltage
```

Example:

```text
Multimeter Voltage  : 2.500V
Firmware Voltage    : 2.450V

ADC_CORRECTION 		= 2.500 / 2.450
					= 1.0204
```

Update the firmware:

```cpp
#define ADC_CORRECTION 1.0204f
```

### Recommended Test Points

For best results, verify several points across the operating range:

```text
0.0V
1.5V
3.0V
5.0V
```

If the calculated voltage closely matches the multimeter reading at all test points, calibration is complete.

### Notes

* Calibration compensates primarily for resistor tolerance.
* Most installations require little or no correction.
* Calibration only needs to be performed once unless hardware is changed.
* The 100nF filter capacitor does not affect DC calibration accuracy.

---

## 📊 ADC Filtering

The ADS1115 reading can be averaged in firmware:

```cpp
#define ADC_SAMPLES 2
```

Current firmware averages the configured number of ADS1115 samples before calculating AFR.
							  
 

Combined with the 1kΩ + 100nF RC filter, this provides stable AFR readings while maintaining good response time.
   

Increase `ADC_SAMPLES` for smoother readings or decrease it for faster response.

---

## 🧰 Hardware Used

* [Ideaspark ESP32 1.9" LCD Board](https://manuals.plus/ae/1005007181435830)
* [ADS1115 16-bit I²C ADC Module](https://www.ti.com/lit/ds/symlink/ads1115.pdf)
* Wideband Controller with 0-5V Analog Output [Example:14Point7 SLC 2](https://www.14point7.com/products/sigma-lambda-controller-free-2>)
* 10kΩ Resistors (×2)
* 1kΩ Resistor
* 100nF Ceramic Capacitor

---

## 📥 Installation

1. Download or clone the repository
2. Download `JetBrainsMonoBold50pt7b.h` and copy to main directory
3. Open the project in Arduino IDE
4. Install required libraries:

   * Adafruit GFX
   * Adafruit ST7789
   * Adafruit ADS1X15
5. Assemble the divider and filter circuit
6. Connect ADS1115 to the ESP32 via I²C
7. Connect the wideband analog output
8. Upload the firmware
9. Power the ESP32
10. Enjoy your new AFR gauge 🚗

---

## ❓ FAQ

### Why is the AFR reading unstable?

Verify:

* The 1kΩ + 100nF filter is installed
* Wideband ground and ESP32 ground are connected together
* ADS1115 ground is connected to the same ground reference
* Wiring is kept reasonably short

### Can I connect a 5V signal directly to ADS1115 A0?

No.

The divider network is required to reduce the wideband's 0-5V output to approximately 0-2.5V before it reaches the ADS1115.

### Why does the displayed AFR not match my controller?

Different wideband controllers may use different voltage-to-AFR mappings.

Adjust the conversion formula in the firmware to match your controller's output specification.

### Want faster or slower display updates?

```cpp
#define FRAME_TIME_MS 50
```

Examples:

```text
100 = 10 FPS
50  = 20 FPS
33  = 30 FPS
```

---

## 📜 License

MIT License

---

## ❤️ Credits

Built for enthusiasts who prefer gauges that are simple, readable, and purpose-built.
