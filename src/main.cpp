#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <PZEM004Tv30.h>
#include <Adafruit_ST7789.h>
#include <string>
#include <math.h>
#include "ST7789_extend.h"
#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include "time.h"
#include "config.h"
#include "secret.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//TFT init
ST7789_extend tft(TFT_CS, TFT_DC, TFT_RST);

//Pzem init
PZEM004Tv30 pzem0(PZEM0_SERIAL, PZEM0_RX_PIN, PZEM0_TX_PIN);
PZEM004Tv30 pzem1(PZEM1_SERIAL, PZEM1_RX_PIN, PZEM1_TX_PIN);

//User data
struct users_data {
    String Room;
    double energy;
    uint16_t bill;

    users_data(const String& room, uint16_t energyVal, uint16_t billVal)
        : Room(room), energy(energyVal), bill(billVal) {}
};

//User data init
users_data A1("P01", 0, 0);
users_data A2("P02", 0, 0);

//RTC init
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

struct RTC_Data {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;

    RTC_Data()
    : year(0), month(0), day(0),
        hour(0), minute(0), second(0),
        dayOfWeek(0) {}

    RTC_Data(uint16_t y, uint8_t m, uint8_t d,
            uint8_t h, uint8_t min, uint8_t s, uint8_t dow)
    : year(y), month(m), day(d),
        hour(h), minute(min), second(s),
        dayOfWeek(dow) {}
    };


//NTP init
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, GMT_OFFSET_SEC);

//firebase object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

unsigned long startAttemptTime = millis();

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

    //Wifi On
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
        Serial.print(".");
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINTLN("Connected Successfully");
        DEBUG_PRINTLN(WiFi.localIP());

        timeClient.begin();
        timeClient.update();
        for (int i=0; i<5; i++) {
            unsigned long epochTime = timeClient.getEpochTime();
            DateTime ntpTime = DateTime(epochTime);
            rtc.adjust(ntpTime);
        }
    
        //Firebase config
        config.api_key = API_KEY;
        config.database_url = DATABASE_URL;

        //Authentication
        auth.user.email = USER_EMAIL;
        auth.user.password = USER_PASSWORD;

        //debug token
        config.token_status_callback = tokenStatusCallback;

        //Firebase init
        Firebase.begin(&config, &auth);
        Firebase.reconnectWiFi(true);
        fbdo.setResponseSize(512);

        //Wifi sleep
        WiFi.setSleep(true);
    } else {
        Serial.println("Cannot connect, try later");
    }
}

void loop() {
    DateTime t = rtc.now();
    RTC_Data now(t.year(), t.month(), t.day(),
                 t.hour(), t.minute(), t.second(),
                 t.dayOfTheWeek());
    DEBUG_PRINT(daysOfWeek[now.dayOfWeek]);
    DEBUG_PRINT(" - ");

    DEBUG_PRINT("Time: ");
    DEBUG_PRINT(now.hour);
    DEBUG_PRINT(":");
    DEBUG_PRINT(now.minute);
    DEBUG_PRINT(":");
    DEBUG_PRINT(now.second);

    DEBUG_PRINT("  Date: ");
    DEBUG_PRINT(now.day);
    DEBUG_PRINT("/");
    DEBUG_PRINT(now.month);
    DEBUG_PRINT("/");
    DEBUG_PRINTLN(now.year);

    tft.deleteText(5, 20, 2, 23);

    tft.print(daysOfWeek[now.dayOfWeek], 5, 20, 2, ST77XX_WHITE);
    tft.print(", ");
    tft.print(now.hour > 9 ? String(now.hour) : "0" + String(now.hour));
    tft.print(":");
    tft.print(now.minute > 9 ? String(now.minute) : "0" + String(now.minute));
    tft.print(":");
    tft.print(now.second > 9 ? String(now.second) : "0" + String(now.second));

    tft.print(" - ");
    tft.print(now.day > 9 ? String(now.day) : "0" + String(now.day));
    tft.print("/");
    tft.print(now.month > 9 ? String(now.month) : "0" + String(now.month));
    tft.print("/");
    tft.print(now.year);

    if (!isnan(pzem0.voltage()))
        A1.energy = pzem0.energy();
    if (!isnan(pzem1.voltage()))
        A2.energy = pzem1.energy();

    A1.bill = A1.energy * UNIT_PRICE;
    A2.bill = A2.energy * UNIT_PRICE;
    
    tft.deleteText(20, 140, 2, 10);
    tft.print(A1.energy, 20, 140, 2, ST77XX_WHITE);
    tft.print(" KWh");
    tft.deleteText(180, 140, 2, 10);
    tft.print(A2.energy, 180, 140, 2, ST77XX_WHITE);
    tft.print(" KWh");
    
    tft.deleteText(10, 200, 2, 12);
    tft.print(A1.bill, 10, 200, 2, ST77XX_WHITE);
    tft.print(" VND");
    tft.deleteText(170, 200, 2, 12);
    tft.print(A2.bill, 170, 200, 2, ST77XX_WHITE);
    tft.print(" VND");
    DEBUG_PRINTLN();
    DEBUG_PRINTF("%.1f V, %.2f A, %.1f W, %.3f kWh, %.1f Hz",
              pzem0.voltage(),
              pzem0.current(),
              pzem0.power(),
              pzem0.energy(),
              pzem0.frequency());
    DEBUG_PRINTLN();
    DEBUG_PRINTF("%.1f V, %.2f A, %.1f W, %.3f kWh, %.1f Hz",
              pzem1.voltage(),
              pzem1.current(),
              pzem1.power(),
              pzem1.energy(),
              pzem1.frequency());
    DEBUG_PRINTLN();

    if (now.second == 0) {
        if (Firebase.RTDB.setFloat(&fbdo,
            "/" + String(ID1) + 
            "/" + String(now.day) + "-" + String(now.month) + "-" + String(now.year) +
            "/Energy",
            roundf(A1.energy * 100) / 100
        )) {
            DEBUG_PRINTLN("Success upload");
        }
        else DEBUG_PRINTLN(fbdo.errorReason());

        if (Firebase.RTDB.setInt(&fbdo,
            "/" + String(ID1) + 
            "/" + String(now.day) + "-" + String(now.month) + "-" + String(now.year) +
            "/Bill",
            A1.bill
        )) {
            DEBUG_PRINTLN("Success upload");
        }
        else DEBUG_PRINTLN(fbdo.errorReason());

        if (Firebase.RTDB.setFloat(&fbdo,
            "/" + String(ID2) + 
            "/" + String(now.day) + "-" + String(now.month) + "-" + String(now.year) +
            "/Energy",
            roundf(A2.energy * 100) / 100
        )) {
            DEBUG_PRINTLN("Success upload");
        }
        else DEBUG_PRINTLN(fbdo.errorReason());
        if (Firebase.RTDB.setInt(&fbdo,
            "/" + String(ID2) + 
            "/" + String(now.day) + "-" + String(now.month) + "-" + String(now.year) +
            "/Bill",
            A2.bill
        )) {
            DEBUG_PRINTLN("Success upload");
        }
        else DEBUG_PRINTLN(fbdo.errorReason());

        WiFi.setSleep(true);
    }
    delay(500);
}
