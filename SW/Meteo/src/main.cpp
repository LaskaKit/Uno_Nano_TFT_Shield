/* LasKKit Home Weather Station. 
 * Read Temperature, Humidity and pressure and show on the display
 * For settings see config.h
 * 
 * Email:obchod@laskarduino.cz
 * Web:laskarduino.cz
 */

#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//#include "Fonts/GFXFF/FreeMono12pt7b.h" // Include the header file attached to this sketch

// TFT SPI
#define TFT_LED   33                  // TFT backlight pin

#define TFT_DISPLAY_RESOLUTION_X 240
#define TFT_DISPLAY_RESOLUTION_Y 320

// Define the colors, color picker here: https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html
#define TFT_TEXT_COLOR              0xFFFF  // white     0xFFFF
#define TFT_BACKGROUND_COLOR        0x00A6  // dark blue 0x00A6
#define TFT_TILE_SHADOW_COLOR       0x0000  // black     0x0000
#define TFT_TILE_BACKGROUND_COLOR_1 0x0700  // green     0x0700
#define TFT_TILE_BACKGROUND_COLOR_2 0x3314  // blue      0x3314
#define TFT_TILE_BACKGROUND_COLOR_3 0xDEC0  // yellow    0xDEC0
#define TFT_TILE_BACKGROUND_COLOR_4 0xD000  // red       0xD000
#define TFT_LED_PWM                 255     // dutyCycle 0-255 last minimum was 15

#define BOOT_MESSAGE "booting..."
#define REFRESH_RATE_MS 60*1000

// Konstanty pro vykresleni dlazdic
#define TILES_OFFSET_Y  60   // Tiles Y start ofset
#define TEXT_PADDING    5    // odsazeni text
#define TILE_MARGIN     10   // odsazeni dlazdicka
#define TILE_SIZE_X (TFT_DISPLAY_RESOLUTION_X - 3 * TILE_MARGIN ) / 2
#define TILE_SIZE_Y (TFT_DISPLAY_RESOLUTION_Y - TILES_OFFSET_Y - 3 * TILE_MARGIN) / 2

// nastavení adresy senzoru
#define BME280_ADRESA (0x76)
// inicializace senzoru BME z knihovny
Adafruit_BME280 bme;

float temp;
int pressure;
int humidity;
int co2;
String date;
uint64_t nextRefresh;
uint64_t nextRefreshDallas;
bool firstLoop = true;

//char buff[512];

TFT_eSPI display = TFT_eSPI();       // Invoke custom library

void readBME(){
  temp = bme.readTemperature();
  Serial.print("Temperature: ");
  Serial.println(temp);

  pressure = bme.readPressure() / 100.0F;   // s přepočtem na hektoPascaly
  Serial.print("Pressure: ");
  Serial.println(pressure);

  humidity = bme.readHumidity();
  Serial.print("Humidity: ");
  Serial.println(humidity);
}

// Samotna funkce pro vykresleni barevne dlazdice
void drawTile(uint8_t position, char title[], char value[], uint16_t color) {
  // Souradnie dlazdice
  uint16_t x = 0;
  uint16_t y = 0;

  // Souradnice dlazdice podle jedne ze ctyr moznych pozic (0 az 3)
  switch (position) {
    case 0:
      x = TILE_MARGIN;
      y = TILES_OFFSET_Y;
      break;
    case 1:
      x = (TILE_MARGIN * 2) + TILE_SIZE_X;
      y = TILES_OFFSET_Y;
      break;
    case 2:
      x = TILE_MARGIN;
      y = TILES_OFFSET_Y + TILE_SIZE_Y + TILE_MARGIN;
      break;
    case 3:
      x = (TILE_MARGIN * 2) + TILE_SIZE_X;
      y = TILES_OFFSET_Y + TILE_SIZE_Y + TILE_MARGIN;
      break;
  }

  display.fillRect(x, y, TILE_SIZE_X, TILE_SIZE_Y, color);

  // Vycentrovani a vykresleni title u dlazdice
  display.setTextSize(2);
  display.setTextFont(1);
  display.setTextDatum(TC_DATUM);
  display.setTextColor(TFT_TEXT_COLOR, color);
  display.drawString(title, x + ((TILE_SIZE_X / 2)), y + TEXT_PADDING);
  
  // Vycentrovani a vykresleni hlavni hodnoty
  display.setTextSize(3);
  display.setTextFont(1);
  display.setTextDatum(MC_DATUM);
  display.setTextColor(TFT_TEXT_COLOR, color);
  display.drawString(value, x + (TILE_SIZE_X / 2), y + (TILE_SIZE_Y / 2));
}

/* Pomocne pretizene funkce pro rozliseni, jestli se jedna o blok
   s promennou celeho cisla, nebo cisla s desetinou carkou
*/
void drawTile(uint8_t position, char title[], float value, uint16_t color) {
  // Prevod ciselne hodnoty float na retezec
  char strvalue[8];
  dtostrf(value, 3, 1, strvalue);
  drawTile(position, title, strvalue, color);
}

void drawTile(uint8_t position, char title[], int value, uint16_t color) {
  // Prevod ciselne hodnoty int na retezec
  char strvalue[8];
  itoa(value, strvalue, 10);
  drawTile(position, title, strvalue, color);
}

void drawScreen() {

  display.fillRect(0, 0, TFT_DISPLAY_RESOLUTION_X, TILES_OFFSET_Y, TFT_BACKGROUND_COLOR); // clear top and time bars background

  // logo laskarduino
 //display.drawBitmap(TFT_DISPLAY_RESOLUTION_X/2-24, TFT_DISPLAY_RESOLUTION_Y/2-24, laskarduino_glcd_bmp, 48, 48, TFT_TEXT_COLOR, TFT_BACKGROUND_COLOR);

  // datum a cas
  display.setTextSize(2);
  display.setTextFont(2);
  display.setTextColor(TFT_WHITE, TFT_BACKGROUND_COLOR);
  display.setTextDatum(MC_DATUM);
  display.drawString(date, TFT_DISPLAY_RESOLUTION_X / 2, 35);

  //draw squares
  drawTile(0, "Tout,`C", temp, TFT_TILE_BACKGROUND_COLOR_1);
  drawTile(1, "Vlh,%", humidity, TFT_TILE_BACKGROUND_COLOR_2);
  drawTile(2, "Tl,hPa", pressure, TFT_TILE_BACKGROUND_COLOR_3);
  drawTile(3, "CO2", co2, TFT_TILE_BACKGROUND_COLOR_4);
  
}

void setup() {

  Serial.begin(9600);
  while(!Serial) {} // Wait until serial is ok

  if (!bme.begin(BME280_ADRESA)) {
    Serial.println("BME280 senzor nenalezen, zkontrolujte zapojeni!");
    while (1);
  }
  
  display.begin();
  display.setRotation(0);
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setTextFont(1);
  display.setTextColor(TFT_YELLOW, TFT_BLACK);
  display.setTextDatum(MC_DATUM);  

  display.drawString(BOOT_MESSAGE, display.width() / 2, display.height() / 2);

}

void loop() {
  
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates


  // Zmer hodnoty, prekresli displej a opakuj za nextRefresh milisekund
  if (millis() > nextRefresh) {
    nextRefresh = millis() + REFRESH_RATE_MS;
    readBME();

    if (firstLoop){
      display.fillScreen(TFT_BACKGROUND_COLOR);
      firstLoop = false;
    }
    drawScreen();
  }
}