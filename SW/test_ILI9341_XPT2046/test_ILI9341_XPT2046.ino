#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"

// nastavení propojovacích pinů
#define TFT_CS    10
#define TFT_DC    9
#define Touch_CS  7

bool start = false;

// Use hardware SPI (on Uno and Nano, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// inicializace řadiče dotykové vrstvy
XPT2046_Touchscreen touch(Touch_CS);

void setup() {

  tft.begin();
  touch.begin();

  tft.setRotation(0); 
  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);

  tft.setTextSize(2);

  tft.println("Touch Screen");
  tft.setTextSize(2);
  tft.println("ILI9341 TFT");
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_ORANGE);
  tft.println("laska");
  tft.println("    rduino.cz");

  tft.drawFastHLine(10, 80, 220, ILI9341_BLUE);
}

void loop() {
  // pokud je detekován dotyk na displeji, nebo se jedná o první spuštění vypíše souřadnice dotyku
  if ((touch.touched()) || !start) {
    start = true;
    // načti do proměnné bod souřadnice dotyku
    TS_Point bod = touch.getPoint();
    
    tft.fillRect(0, 100, 200, 50, ILI9341_BLACK);
    tft.setCursor(0, 100);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);

    tft.print("Pressure = ");
    tft.println(bod.z);
    tft.print("       X = ");
    tft.println(bod.x);
    tft.print("       Y = ");
    tft.println(bod.y);
  }
  delay(1);

}
