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

#define TFT_MOSI  		23
#define TFT_SCLK  		18
#define TFT_CS    		15
#define TFT_DC    		2
#define TFT_RST   		4
#define BL        		32
#define SCREEN_W	 	320
#define SCREEN_H	 	170
#define FRAME_TIME_MS 	33

#define WB_PIN 	  		33
#define V_DIVIDE  		2.0f
#define AFR_DIVIDE  	2.0f
#define ADC_MAX_COUNTS 	4095.0f
#define ADC_REF_VOLTAGE 3.3f
#define ADC_GAIN		1.0000f	//GPIO CALIBRATION AND OFFSET ERROR

#define BG_COLOR  		0x0841
#define TXT_COLOR 		ST77XX_WHITE
#define AFR_MIN		 	10.0f
#define AFR_MAX		 	20.0f
#define AFR_RICH	 	12.0f
#define AFR_STOICH	 	15.0f
#define ADC_SAMPLES		16

#define NUM_BOXES 		11
#define GAP 			2
#define BOX_HEIGHT 		28
#define INACTIVE_COLOR  0x0861

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(SCREEN_W, SCREEN_H);

int textX;
int textY;
char buf[10];

void drawTopBoxes(GFXcanvas16 &c, float afr) {	
    int activeBoxes;
	uint16_t activeColor;

	if (afr < AFR_MIN) {
        activeBoxes = 0;
	} else {
        activeBoxes = min(NUM_BOXES, (int)(afr - (AFR_MIN - 1.0f)));
	}
	
	if (afr < AFR_RICH) {
		activeColor = ST77XX_BLUE;
	} else if (afr < AFR_STOICH) {
		activeColor = ST77XX_GREEN;
	} else {
		activeColor = ST77XX_RED;
	}

    for (int i = 0; i < NUM_BOXES; i++) {                
        int x1 = (SCREEN_W * i) / NUM_BOXES;
        int x2 = (SCREEN_W * (i + 1)) / NUM_BOXES;

        x1 += GAP / 2;
        x2 -= GAP / 2;
        int w = x2 - x1;

        c.fillRoundRect( x1, 4, w, BOX_HEIGHT, 4, (i < activeBoxes) ? activeColor : INACTIVE_COLOR );
    }
}

void setup() {
    Serial.begin(115200);

    tft.init(SCREEN_H, SCREEN_W);
    tft.setRotation(3);
    tft.fillScreen(BG_COLOR);
	
	//analogReadResolution(12);
	pinMode(WB_PIN, INPUT);
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
    canvas.getTextBounds("00.00", 0, 0, &x1, &y1, &w, &h);

    textX = (SCREEN_W - w) / 2 - x1;
    textY = ((SCREEN_H - 36) - h) / 2 - y1 + 36;
}

void loop() {  
	uint32_t start_loop	= millis();
	uint32_t sum 		= 0;
	float afr_value		= 0.0f;	
	
	for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += analogRead(WB_PIN);
    }
    uint16_t raw 	= (sum / ADC_SAMPLES);
	float voltage	= (raw / ADC_MAX_COUNTS * ADC_REF_VOLTAGE * ADC_GAIN);
	float wbVoltage = (voltage * V_DIVIDE);
	afr_value 		= (AFR_MIN + (wbVoltage * AFR_DIVIDE));
	
	Serial.printf( "raw=%u gpio=%.3fV wb=%.3fV AFR=%.2f\n", raw, voltage, wbVoltage, afr_value ); // DEBUG
	
	if(afr_value < AFR_MIN || afr_value > AFR_MAX) {
		strcpy(buf, "ERR");
		afr_value = 0.0f;
	} else {
		snprintf(buf, sizeof(buf), "%.2f", afr_value);			
	}
	canvas.fillScreen(BG_COLOR);
	drawTopBoxes(canvas, afr_value);
	canvas.setCursor(textX, textY);
	canvas.print(buf);
	tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);	

	uint32_t end_loop = (millis() - start_loop);
    if (end_loop < FRAME_TIME_MS) {
		delay(FRAME_TIME_MS - end_loop); //30FPS. ADJUST AS NEEDED. 
	}	
}
