# ESP32 Digital Wideband AFR Gauge

![AFR Gauge](https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/gauge.jpg)

A simple ESP32-based digital AFR gauge for wideband controllers that provide a **0-5V analog output**. AFR is displayed on a **1.9" 320×170 ST7789 TFT display** with a large numeric readout and color-coded AFR indicator bar.

> Tested with a 0-5V linear wideband analog output. AFR conversion may require adjustment for controllers using a different voltage-to-AFR mapping.

---

## Features

- 1.9" easy-to-read AFR display
- Large custom JetBrains Mono font
- Color-coded AFR status bar
- 16-sample ADC averaging for stable readings
- RC-filtered analog input
- Designed for common 0-5V wideband controller outputs
- ESP32 + ST7789 based

---

## Hardware

### Main Components

This project was developed and tested using:

- Ideaspark ESP32 1.9" LCD Board
- ESP32-WROOM module
- 1.9" 170×320 ST7789 TFT display
- Wideband controller with a 0-5V analog output

### Board

![Ideaspark ESP32 LCD Board]<https://manuals.plus/ae/1005007181435830>

### Pinout

![Ideaspark ESP32 Pinout](https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/pinout.png)

### Pin Assignment

| Function | GPIO |
|----------|------|
| TFT MOSI | GPIO23 |
| TFT SCLK | GPIO18 |
| TFT CS | GPIO15 |
| TFT DC | GPIO2 |
| TFT RST | GPIO4 |
| TFT Backlight | GPIO32 |
| Wideband Analog Input | GPIO33 |

### Wideband Input Circuit

⚠️ **ESP32 ADC inputs are NOT 5V tolerant.**

If your wideband controller outputs a 0-5V analog signal, **do not connect it directly to GPIO33 (or any ESP32 ADC pin).**

Use the following voltage divider and RC filter:

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

| Component | Quantity | Notes |
|------------|----------|--------|
| 10kΩ resistor | 2 | 1% metal film recommended |
| 1kΩ resistor | 1 | 1-5% metal film |
| 100nF capacitor | 1 | Ceramic, ≥16V |

### ADC Scaling

```text
Wideband Output : 0.0V → 5.0V
ESP32 ADC Input : 0.0V → 2.5V
```

The 10kΩ / 10kΩ divider scales the wideband's 0-5V output to approximately 0-2.5V.

The 1kΩ resistor and 100nF capacitor form a low-pass filter that reduces noise and improves ADC stability.

Failure to use a voltage divider may permanently damage the ESP32.

---

## Display Font

This project uses a custom **JetBrains Mono Bold 50pt** font for the AFR display.

Required file:

- JetBrainsMonoBold50pt7b.h

Download:

https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/JetBrainsMonoBold50pt7b.h

Place the file in your project directory before compiling.

---

## AFR Calibration

The current firmware assumes:

```text
0.0V = 10.00 AFR
5.0V = 20.00 AFR
```

If your wideband controller uses a different analog output scale, modify the AFR conversion formula in the source code accordingly.

Current firmware conversion:

```cpp
float voltage   = ((raw / 4095.0f) * 3.3f);
float wbVoltage = (voltage * 2.0f);
float afr       = (10.0f + (wbVoltage * 2.0f));
```

---

## ADC Filtering

To improve display stability and reduce noise from the wideband controller's analog output, the firmware averages 16 ADC samples before calculating AFR.

```cpp
uint32_t sum = 0;

for (int i = 0; i < 16; i++) {
    sum += analogRead(WB_PIN);
}

int raw = sum / 16;
```

This software averaging works together with the RC input filter (1kΩ + 100nF) to provide a smoother AFR reading and reduce display flicker.

---

## Enclosure

![Enclosure](https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/enclosure.jpg)

### Downloads

- https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/case.stl
- https://raw.githubusercontent.com/danuecumaster/ESP32-Digital-Wideband-AFR-Gauge/main/assets/cover.stl

### Recommended Print Material

- ABS (preferred for automotive interiors)
- ASA (also suitable)

PLA is not recommended due to the high temperatures commonly reached inside parked vehicles.

---

## License

MIT License