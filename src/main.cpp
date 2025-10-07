#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <PZEM004Tv30.h>
#include <Adafruit_ST7789.h>
#include <string>
#include <math.h>
#include "ST7789_extend.h"
#include <vector>
#include "config.h"
#include <Wire.h>
#include <RTClib.h>

ST7789_extend tft(TFT_CS, TFT_DC, TFT_RST);

PZEM004Tv30 pzem0(PZEM0_SERIAL, PZEM0_RX_PIN, PZEM0_TX_PIN);
PZEM004Tv30 pzem1(PZEM1_SERIAL, PZEM1_RX_PIN, PZEM1_TX_PIN);


struct users_data {
    String Room;
    double energy;
    uint16_t bill;

    users_data(const String& room, uint16_t energyVal, uint16_t billVal)
        : Room(room), energy(energyVal), bill(billVal) {}
};

users_data A1("P01", 0, 0);
users_data A2("P02", 0, 0);

uint16_t unit_price = UNIT_PRICE;

RTC_DS1307 rtc;

const char* daysOfWeek[] = {
  "Sun",    // 0
  "Mon",    // 1
  "Tue",   // 2
  "Wed", // 3
  "Thu",  // 4
  "Fri",    // 5
  "Sat"   // 6
};

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    rtc.begin();
    tft.init(240, 320); 
    tft.invertDisplay(0);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);
    tft.initTable(0, 0, 320, 50, 1, 1, ST77XX_BLUE);
    tft.addTable(240 - 50, 3, 2, ST77XX_BLUE);
    tft.print(A1.Room, 60, 70, 2, ST77XX_WHITE);
    tft.print(A2.Room, 220, 70, 2, ST77XX_WHITE);
}

void loop() {
    DateTime now = rtc.now();
    Serial.print(daysOfWeek[now.dayOfTheWeek()]);
    Serial.print(" - ");

    Serial.print("Time: ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.print(now.second());

    Serial.print("  Date: ");
    Serial.print(now.day());
    Serial.print("/");
    Serial.print(now.month());
    Serial.print("/");
    Serial.println(now.year());

    tft.deleteText(5, 20, 2, 23);

    tft.print(daysOfWeek[now.dayOfTheWeek()], 5, 20, 2, ST77XX_WHITE);
    tft.print(", ");
    tft.print(now.hour() > 9 ? String(now.hour()) : "0" + String(now.hour()));
    tft.print(":");
    tft.print(now.minute() > 9 ? String(now.minute()) : "0" + String(now.minute()));
    tft.print(":");
    tft.print(now.second() > 9 ? String(now.second()) : "0" + String(now.second()));

    tft.print(" - ");
    tft.print(now.day() > 9 ? String(now.day()) : "0" + String(now.day()));
    tft.print("/");
    tft.print(now.month() > 9 ? String(now.month()) : "0" + String(now.month()));
    tft.print("/");
    tft.print(now.year());

    A1.energy = pzem0.energy();
    A2.energy = pzem1.energy();

    A1.bill = A1.energy * unit_price;
    A2.bill = A2.energy * unit_price;

    tft.print(A1.energy, 20, 140, 2, ST77XX_WHITE);
    tft.print(" KWh");
    tft.print(A2.energy, 180, 140, 2, ST77XX_WHITE);
    tft.print(" KWh");
    
    tft.print(A1.bill, 10, 200, 2, ST77XX_WHITE);
    tft.print(" VND");
    tft.print(A2.bill, 170, 200, 2, ST77XX_WHITE);
    tft.print(" VND");
    Serial.println();
    Serial.printf("%.1f V, %.2f A, %.1f W, %.3f kWh, %.1f Hz",
              pzem0.voltage(),
              pzem0.current(),
              pzem0.power(),
              pzem0.energy(),
              pzem0.frequency());
    Serial.println();
    Serial.printf("%.1f V, %.2f A, %.1f W, %.3f kWh, %.1f Hz",
              pzem1.voltage(),
              pzem1.current(),
              pzem1.power(),
              pzem1.energy(),
              pzem1.frequency());
    Serial.println();
    delay(1000);
}
