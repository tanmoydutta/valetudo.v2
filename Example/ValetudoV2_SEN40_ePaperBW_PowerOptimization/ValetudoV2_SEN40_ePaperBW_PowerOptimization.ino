#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>

#include "Adafruit_SHT4x.h"


#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

//GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=D8*/ D2, /*DC=D3*/ D4, /*RST=D4*/ D5, /*BUSY=D2*/ D3)); // GDEP015OC1 200x200, IL3829, no longer available
GxEPD2_BW<GxEPD2_154_M09, GxEPD2_154_M09::HEIGHT> display(GxEPD2_154_M09(/*CS=D8*/ D2, /*DC=D3*/ D4, /*RST=D4*/ D5, /*BUSY=D2*/ D3)); // GDEW0154M09 200x200, JD79653A --Good Display
//GxEPD2_3C<GxEPD2_266c, GxEPD2_266c::HEIGHT> display(GxEPD2_266c(/*CS=D8*/ D2, /*DC=D3*/ D4, /*RST=D4*/ D5, /*BUSY=D2*/ D3)); // GDEY0266Z90 152x296, SSD1680 -- Red Black White
int counter = 0;
int refreshCounter = 0;

void setup()
{  
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  
  
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  delay(100);

  Serial.println("Adafruit SHT4x test");
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_LOW_PRECISION);

  sht4.setHeater(SHT4X_NO_HEATER);

  display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  //display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  //display.init(115200, true, 10, false, SPIn, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // extended init method with SPI channel and/or settings selection
 /* if (display.pages() > 1)
  {
    delay(100);
    Serial.print("pages = "); Serial.print(display.pages()); Serial.print(" page height = "); Serial.println(display.pageHeight());
    delay(1000);
  }*/
  display.clearScreen(); //return;
  // first update should be full refresh
  //display.powerOff();
  delay(500);
  

}

void loop()
{
  //deepSleepTest();
  viewSensorData();
}

void viewSensorData() 
{
  sensors_event_t humidity, temp;
  float battLevel= analogRead(A0);
  
  battLevel *= 2;    // we divided by 2, so multiply back
  battLevel *= 3.0;  // Multiply by 3.6V, our reference voltage
  battLevel /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(battLevel);

  //uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  //timestamp = millis() - timestamp;

  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  //Serial.print("Read duration (ms): ");
  //Serial.println(timestamp);
  
  
  //Serial.println("deepSleepTest");
  const char temperatureText[] = "T:13.06C";
  const char humidityText[] = "H:40.00%";
  const char battText[] = "BAT:0.00V";
  const char countText[] = "Count:99999";

  digitalWrite(D6, LOW);
  //display.init(115200);
  
  if (refreshCounter > 50) {
    refreshCounter = 0;
    display.clearScreen();
  }

  display.setRotation(1);
  //display.setFont(&FreeMonoBold9pt7b);
  if (display.epd2.WIDTH < 104) display.setFont(0);
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);
  int16_t tbx, tby; uint16_t tbw, tbh;
  // center text
  
  display.getTextBounds(temperatureText, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t wx = (display.width() - tbw) / 2;
  uint16_t wy = ((display.height() / 3) - tbh / 2) - tby; // y is base line!
  display.setPartialWindow(wx, wy, tbw, tbh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(wx, wy);
    display.print("T:");
    display.print(temp.temperature);
    display.print("C");    
  }while (display.nextPage());

  display.getTextBounds(humidityText, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t fx = (display.width() - tbw) / 2;
  uint16_t fy = ((display.height() * 2 / 3) - tbh / 2) - tby; // y is base line!
  display.setPartialWindow(fx, fy, tbw, tbh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(fx, fy);
    display.print("H:");
    display.print(humidity.relative_humidity);
    display.print("%");
  } while (display.nextPage());

  //Battery Voltage
  display.setTextSize(2);
  display.getTextBounds(battText, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t gx = 10; //(display.width() - tbw) / 2;
  uint16_t gy = 10;
  display.setPartialWindow(gx, gy, tbw, tbh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(gx, gy); //display.setCursor(10, 20);
    display.print("BAT:");
    display.print(battLevel);display.print("V");

  } while (display.nextPage());

  //Count
  display.getTextBounds(countText, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t hx = 10;
  uint16_t hy = 180;
  display.setPartialWindow(hx, hy, tbw, tbh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    display.setCursor(hx, hy); //display.setCursor(10, 190);
    display.print("Count:");
    display.print(++counter);
  } while (display.nextPage());

  //display.hibernate();
  display.powerOff();

  digitalWrite(D6, HIGH);
  /*digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);

  delay(50);

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);*/
  refreshCounter++;

  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);

  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);

  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);
  delay(60000);

}


