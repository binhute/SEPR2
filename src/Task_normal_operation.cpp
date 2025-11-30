#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Wire.h>
#include <string>
#include <ctype.h>
#include <Preferences.h>
#include "config.h"
#include <PZEM004Tv30.h>
#include "ST7789_extend.h"
#include <RTClib.h>
#include <Task_normal_operation.h>


long tieredElectricCalculate(int kwh, ElectricityTier tiers[], int size, bool includeVAT) {
    long total = 0;
    int remaining = kwh;

    for (int i = 0; i < size; i++) {
        if (remaining <= 0) break;

        int used = remaining;
        if (tiers[i].limit != -1 && used > tiers[i].limit) {
            used = tiers[i].limit;
        }

        total += (long)used * tiers[i].price;
        remaining -= used;
    }

    if (includeVAT) {
        total += total * 0.08; // vat = 8%
    }

    return total;
}

void TaskNormalOpe(void *parameter) {
    //Flash init
    
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
    users_data A1(cfg.room1, 0, 0);
    users_data A2(cfg.room2, 0, 0);

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

    char timeChar[40];
    char old_time[40] = "";
    char RTDB_time[40];

    ElectricityTier tiers[] = {
        {50,   1806},   // tier 1
        {50,   1866},   // tier 2
        {100,  2167},   // tier 3
        {100,  2729},   // tier 4
        {100,  3050},   // tier 5
        {-1,   3151},   // tier 6
    };

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

    //Write data to Flash
    if (!isnan(pzem0.voltage()) || !isnan(pzem1.voltage())) {
        prefs.begin("storage", false);
        if (!isnan(pzem0.voltage())) {
            prefs.putDouble("energy_KH1", pzem0.energy());
            DEBUG_PRINTLN("Writed KH1");
        }

        if (!isnan(pzem1.voltage())) {
            prefs.putDouble("energy_KH2", pzem1.energy());
            DEBUG_PRINTLN("Writed KH2");
        }
        prefs.end();
    }

    //Read data from Flash
    prefs.begin("storage", true);
    A1.energy = prefs.getDouble("energy_KH1", 0);
    A2.energy = prefs.getDouble("energy_KH2", 0);
    prefs.end();
    DEBUG_PRINTLN(A1.energy);
    DEBUG_PRINTLN(A2.energy);

    for (;;) {
        DateTime t = rtc.now();
        RTC_Data now(t.year(), t.month(), t.day(),
                    t.hour(), t.minute(), t.second(),
                    t.dayOfTheWeek());
        
        sprintf(timeChar, "%s, %02d/%02d/%04d, %02d:%02d:%02d",
                daysOfWeek[now.dayOfWeek],
                now.day,
                now.month,
                now.year,
                now.hour,
                now.minute,
                now.second
        );
        
        if (strcmp(timeChar, old_time) != 0) {
            DEBUG_PRINTLN(timeChar);
            tft.deleteText(5, 20, 2, 29);
            tft.print(timeChar, 5, 20, 2, ST77XX_WHITE);
            strcpy(old_time, timeChar);
        }
        
        if (!isnan(pzem0.voltage()))
            A1.energy = pzem0.energy();
        if (!isnan(pzem1.voltage()))
            A2.energy = pzem1.energy();

        if (cfg.unitOrTier == "tier") {
            A1.bill = tieredElectricCalculate(A1.energy, tiers, 6, true);
            A2.bill = tieredElectricCalculate(A2.energy, tiers, 6, true);
        }
        else if (cfg.unitOrTier == "unit") {
            A1.bill = A1.energy * UNIT_PRICE;
            A2.bill = A2.energy * UNIT_PRICE;
        }

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

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}