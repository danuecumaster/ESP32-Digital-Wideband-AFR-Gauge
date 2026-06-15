// -----------------------------------------------------------------------------
//   WB 0-5V
//      |
//     10K
//      |
//      +---- 1K ----+----> ADS1115 A0 ----> ESP32 I2C
//      |            |		  |				 
//     10K         100NF      | 			 
//      |            |		  |				 
//     GND          GND		 GND			
//
// DIVIDER: 10K / 10K  -> 0-5V BECOMES APPROXIMATELY 0-2.5V
// FILTER : 1K + 100NF -> REDUCES ADC NOISE
//
// SEE README.MD FOR WIRING DETAILS.
// -----------------------------------------------------------------------------

/**** LIBRARIES ****/
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "JetBrainsMonoBold50pt7b.h"
/**** LIBRARIES ****/

/**** DISPLAY PINS ****/
#define TFT_MOSI  		23
#define TFT_SCLK  		18
#define TFT_CS    		15
#define TFT_DC    		2
#define TFT_RST   		4
#define BL        		32
/**** DISPLAY PINS ****/

/**** I2C PINS ****/
#define SDA_PIN   		21
#define SCL_PIN   		22
/**** I2C PINS ****/

/**** DISPLAY SETTINGS ****/
#define SCREEN_W	 	320
#define SCREEN_H	 	170
#define FRAME_TIME_MS 	50 				// 100 = 10FPS, 50 = 20FPS, 33 = 30FPS
/**** DISPLAY SETTINGS ****/

/**** ADC SETTINGS ****/
#define ADC_SAMPLES		2				// NUMBER OF ADC SAMPLES
#define ADC_CORRECTION	1.0000f			// ADC CALIBRATION FACTOR
#define V_MULTIPLIER	2.0f			// COMPENSATE FOR 10K/10K DIVIDER
#define WBO_MULTIPLIER	2.0f			// WBO CONTROLLER MULTIPLIER 
#define AFR_TOLERANCE	0.5f			// ERROR THRESHOLD
/**** ADC SETTINGS ****/

/**** AFR BOX SETTINGS ****/
#define BG_COLOR  		0x0841
#define TXT_COLOR 		ST77XX_WHITE
#define AFR_MIN		 	10.0f
#define AFR_MAX		 	20.0f
#define AFR_RICH	 	12.0f
#define AFR_STOICH	 	15.0f
#define NUM_BOXES 		11
#define GAP 			2
#define BOX_HEIGHT 		28
#define INACTIVE_COLOR  0x0861
/**** AFR BOX SETTINGS ****/

/**** DEBUG ****/
#define DEBUG_MODE  	1 				// 1 = ON, 0 = OFF
/**** DEBUG ****/

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(SCREEN_W, SCREEN_H);

Adafruit_ADS1115 ads;

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

    pinMode(BL, OUTPUT);
    digitalWrite(BL, HIGH);

    canvas.setFont(&JetBrainsMono_Bold50pt7b);
    canvas.setTextColor(TXT_COLOR);

    int16_t x1, y1;
    uint16_t w, h;
    canvas.getTextBounds("00.00", 0, 0, &x1, &y1, &w, &h);

    textX = (SCREEN_W - w) / 2 - x1;
    textY = ((SCREEN_H - 36) - h) / 2 - y1 + 36;
	
	Wire.begin(SDA_PIN, SCL_PIN);
	Wire.setClock(400000);
	if (!ads.begin()) {
        Serial.println("ADS1115 not found");
        while (1);
    }
	ads.setGain(GAIN_ONE);
	Serial.println("ADS1115 initialized");
}

void loop() {  
	uint32_t startLoop	= millis();	
	float sum 			= 0.0f;
	float afr			= 0.0f;
	
	for (int i = 0; i < ADC_SAMPLES; i++) {
		int16_t adc = ads.readADC_SingleEnded(0);
        sum += ads.computeVolts(adc);
    }
	
    float rawV			= ((float)sum / ADC_SAMPLES);
	float voltage		= (rawV * V_MULTIPLIER * ADC_CORRECTION);
	afr					= (AFR_MIN + (voltage * WBO_MULTIPLIER));
	
	#if DEBUG_MODE
		Serial.printf("ADC=%.6f WB=%.4fV AFR=%.2f\n", rawV, voltage, afr);
	#endif
	
	if(afr < (AFR_MIN - AFR_TOLERANCE) || afr > (AFR_MAX + AFR_TOLERANCE)) {
		strcpy(buf, "ERROR");
		afr = 0.0f;
	} else {
		snprintf(buf, sizeof(buf), "%.2f", afr);			
	}
	canvas.fillScreen(BG_COLOR);
	drawTopBoxes(canvas, afr);
	canvas.setCursor(textX, textY);
	canvas.print(buf);
	tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);	

	uint32_t endLoop = (millis() - startLoop);
    if (endLoop < FRAME_TIME_MS) {
		delay(FRAME_TIME_MS - endLoop);
	}	
}