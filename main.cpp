// -----------------------------------------------------------------------------
// Wideband Analog Input (GPIO33)
//
// ESP32 ADC inputs are NOT 5V tolerant.
// Do NOT connect it directly to GPIO33.
//
//   WB 0-5V
//      |
//     10k
//      |
//      +---- 1k ----+----> GPIO33
//      |            |
//     10k         100nF
//      |            |
//     GND          GND
//
// Divider: 10k / 10k  -> 0-5V becomes approximately 0-2.5V
// Filter : 1k + 100nF -> reduces ADC noise
//
// See README.md for wiring details.
// -----------------------------------------------------------------------------

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "JetBrainsMonoBold50pt7b.h"

#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_CS    15
#define TFT_DC    2
#define TFT_RST   4
#define BL        32
#define BG_COLOR  0x0841
#define TXT_COLOR ST77XX_WHITE
#define WB_PIN 	  33

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(320, 170);

int textX;
int textY;
char buf[10];

void drawTopBoxes(GFXcanvas16 &c, float afr) {
	const int numBoxes 		= 11;
    const int gap 			= 2;
    const int boxHeight 	= 28;
    uint16_t inactiveColor  = 0x0861;
    int activeBoxes;

	if (afr < 10.0f) {
        activeBoxes = 0;
	} else {
        activeBoxes = min(11, (int)(afr - 9.0));
	}

    for (int i = 0; i < numBoxes; i++) {
        uint16_t activeColor;
        
        if (afr < 12.0f) {
            activeColor = ST77XX_BLUE;
		} else if (afr < 15.0f) {
            activeColor = ST77XX_GREEN;
		} else {
            activeColor = ST77XX_RED;
		}

        int x1 = (320 * i) / numBoxes;
        int x2 = (320 * (i + 1)) / numBoxes;

        x1 += gap / 2;
        x2 -= gap / 2;

        int w = x2 - x1;

        c.fillRoundRect(
            x1,
            4,
            w,
            boxHeight,
            4,
            (i < activeBoxes) ? activeColor : inactiveColor
        );
    }
}

void setup() {
    Serial.begin(115200);

    tft.init(170, 320);
    tft.setRotation(3);
    tft.fillScreen(BG_COLOR);
	
	//analogReadResolution(12);
	analogSetWidth(12);	
    analogSetPinAttenuation(WB_PIN, ADC_11db);
	analogRead(WB_PIN);
	delay(10);

    pinMode(BL, OUTPUT);
    digitalWrite(BL, HIGH);

    canvas.setFont(&JetBrainsMono_Bold50pt7b);
    canvas.setTextColor(TXT_COLOR);

    int16_t x1, y1;
    uint16_t w, h;
    canvas.getTextBounds("20.00", 0, 0, &x1, &y1, &w, &h);

    textX = (320 - w) / 2 - x1;
    textY = ((170 - 36) - h) / 2 - y1 + 36;
}

void loop() {   
	uint32_t sum 	= 0;
	float afr_value = 0.0f;
	
	for (int i = 0; i < 16; i++) {
        sum += analogRead(WB_PIN);
    }
    int raw 		= (sum / 16);
	float voltage	= ((raw / 4095.0f) * 3.3f);
	float wbVoltage = (voltage * 2.0f);
	afr_value 		= (10.0f + (wbVoltage * 2.0f));
	
	if(afr_value < 10.00 || afr_value > 20.00) {
		strcpy(buf, "ERR");
		afr_value = 0.0f;
	} else {
		snprintf(buf, sizeof(buf), "%.2f", afr_value);			
	}
	canvas.fillScreen(BG_COLOR);
	drawTopBoxes(canvas, afr_value);
	canvas.setCursor(textX, textY);
	canvas.print(buf);
	tft.drawRGBBitmap(0, 0, canvas.getBuffer(), 320, 170);	
	
    delay(25);
}
