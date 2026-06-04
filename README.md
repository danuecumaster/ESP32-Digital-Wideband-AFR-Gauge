# ESP32 Digital Wideband AFR Gauge

A simple ESP32-based digital AFR gauge for wideband controllers that provide a **0-5V analog output**. AFR is displayed on a **320×170 ST7789 TFT display** with a large numeric readout and color-coded AFR indicator bar.

![Gauge Photo](images/gauge.jpg)

## Features

* Large, easy-to-read AFR display
* Color-coded AFR status bar
* Moving-average ADC sampling for improved stability
* Designed for common 0-5V wideband controller outputs
* ESP32 + ST7789 based

## Hardware Used

This project was developed and tested using:

* Ideaspark ESP32 1.9" LCD Board
* 170×320 ST7789 TFT display
* ESP32-WROOM module

### Pin Assignment

| Function              | GPIO   |
| --------------------- | ------ |
| TFT MOSI              | GPIO23 |
| TFT SCLK              | GPIO18 |
| TFT CS                | GPIO15 |
| TFT DC                | GPIO2  |
| TFT RST               | GPIO4  |
| TFT Backlight         | GPIO32 |
| Wideband Analog Input | GPIO33 |

![Ideaspark Pinout](images/ideaspark_pinout.png)

## Wideband Input Circuit

### Important

⚠️ **ESP32 ADC inputs are NOT 5V tolerant.**

If your wideband controller outputs a 0-5V analog signal, **do not connect it directly to GPIO33 (or any ESP32 ADC pin).**

A voltage divider is required to reduce the signal to a safe voltage for the ESP32 ADC.

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

### Input Components

* 10kΩ resistor ×2 (1% metal film recommended)
* 1kΩ resistor ×1 (1% metal film recommended)
* 100nF ceramic capacitor (X7R or similar, ≥16V)

### How It Works

* The 10k / 10k divider scales the wideband's 0-5V output to approximately 0-2.5V.
* The 1k resistor and 100nF capacitor form a low-pass filter that reduces noise and improves ADC stability.
* The software compensates for the divider ratio when calculating AFR.

### ADC Scaling

```text
Wideband Output: 0.0V → 5.0V
ESP32 ADC Input: 0.0V → 2.5V
```

Failure to use a voltage divider may permanently damage the ESP32.

## AFR Calibration

The current firmware assumes:

```text
0.0V = 10.00 AFR
5.0V = 20.00 AFR
```

If your wideband controller uses a different analog output scale, modify the AFR conversion formula in the source code accordingly.

## Enclosure

A 3D printable enclosure is available here:

<LINK_TO_STL>

Recommended print material:

* ABS (preferred for automotive interiors)
* ASA (also suitable)

PLA is not recommended due to the high temperatures commonly reached inside parked vehicles.

## License

MIT License
