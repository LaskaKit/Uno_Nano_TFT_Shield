#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"

// nastavení propojovacích pinů
#define LCD_cs 10
#define LCD_dc 9
#define LCD_rst 8
#define DOTYK_cs  7

bool start = false;

// inicializace LCD displeje z knihovny
Adafruit_ILI9341 displej = Adafruit_ILI9341(LCD_cs, LCD_dc, LCD_rst);
// inicializace řadiče dotykové vrstvy z knihovny
XPT2046_Touchscreen dotyk(DOTYK_cs);

void setup() {
  // zahájení komunikace s displejem a dotykovou vrstvou
  displej.begin();
  dotyk.begin();
  // pro otočení displeje stačí změnit číslo
  // v závorce v rozmezí 0-3
  displej.setRotation(0);
  // vyplnění displeje černou barvou
  displej.fillScreen(ILI9341_BLACK);
  // nastavení kurzoru na souřadnice x, y
  displej.setCursor(0, 0);
  displej.setTextColor(ILI9341_WHITE);
  // velikost textu lze nastavit změnou čísla v závorce
  displej.setTextSize(2);
  // funkce pro výpis textu na displej,
  // print tiskne na řádek stále za sebou,
  // println na konci textu přeskočí na nový řádek
  displej.println("Dotykovy displej");
  displej.setTextSize(2);
  displej.println("TFT LCD");
  displej.setTextSize(2);
  displej.println("laskarduino.cz");
  // funkce pro vykreslení vodorovné čáry,
  // zadání (výchozí bod x, výchozí bod y, délka čáry, barva čáry)
  displej.drawFastHLine(10, 70, 220, ILI9341_BLUE);
}

void loop() {
  // pokud je detekován dotyk na displeji, nebo se jedná o první spuštění vypíše souřadnice dotyku
  if ((dotyk.touched()) || !start) {
    start = true;
    // načti do proměnné bod souřadnice dotyku
    TS_Point bod = dotyk.getPoint();
    
    displej.fillRect(0, 100, 200, 50, ILI9341_BLACK);
    displej.setCursor(0, 100);
    displej.setTextColor(ILI9341_WHITE);
    displej.setTextSize(2);
    // vypsání informací o souřadnicích posledního dotyku
    // včetně tlaku dotyku - tlak není přiliš přesný
    displej.print("Tlak = ");
    displej.println(bod.z);
    displej.print("   X = ");
    displej.println(bod.x);
    displej.print("   Y = ");
    displej.println(bod.y);
  }
  delay(1);

}
