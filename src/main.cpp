#include <Preferences.h>
#include <Wire.h>
#include <SPI.h>
#include <string>
#include <ctype.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <LittleFS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "ST7789_extend.h"
#include <PZEM004Tv30.h>
#include <ESPAsyncWebServer.h>
#include <math.h>
#include <RTClib.h>
#include <WiFi.h>
#include "time.h"
#include "config.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Task_normal_operation.h"
#include "Task_wifi_cloud.h"

AsyncWebServer server(80);
Preferences prefs;

//Task
TaskHandle_t normalOpe_handle = NULL;
TaskHandle_t wifiCloud_handle = NULL;

//Queue
QueueHandle_t firebaseUpload;

bool isConfigSaved() {
    if (!prefs.begin("config", true)) {
        prefs.end();
        return false;
    }

    bool hasSSID      = prefs.isKey("ssid");
    bool hasPass      = prefs.isKey("wifipass");
    bool hasFBurl     = prefs.isKey("fburl");
    bool hasFBapi     = prefs.isKey("fbapi");
    bool hasEmail     = prefs.isKey("email");
    bool hasEmailPass = prefs.isKey("emailpass");
    bool hasRoom1     = prefs.isKey("room1");
    bool hasRoom2     = prefs.isKey("room2");
    bool hasUnitTier  = prefs.isKey("unitOrTier");
    bool unitPrice    = prefs.isKey("unitPrice");

    prefs.end();

    return (
        hasSSID && hasPass &&
        hasFBurl && hasFBapi &&
        hasEmail && hasEmailPass &&
        hasRoom1 && hasRoom2 &&
        hasUnitTier && unitPrice
    );
}

void startAP(const String &ssid, const String &password) {
    DEBUG_PRINTLN("Starting AP mode...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password.c_str());

    DEBUG_PRINTLN("AP IP: ");
    DEBUG_PRINTLN(WiFi.softAPIP());

    if (!LittleFS.begin(true)) {
        DEBUG_PRINTLN("LittleFS mount failed!");
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.serveStatic("/", LittleFS, "/");

    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
        prefs.begin("config");
        prefs.putString("ssid",       request->arg("ssid"));
        prefs.putString("wifipass",   request->arg("wifipass"));
        prefs.putString("fburl",      request->arg("fburl"));
        prefs.putString("fbapi",      request->arg("fbapi"));
        prefs.putString("email",      request->arg("email"));
        prefs.putString("emailpass",  request->arg("emailpass"));
        prefs.putString("room1",      request->arg("room1"));
        prefs.putString("room2",      request->arg("room2"));
        prefs.putString("unitOrTier", request->arg("unitOrTier"));
        prefs.putString("unitPrice",  request->arg("unitPrice"));
        prefs.end();

        request->send(200, "text/plain", "Saved! Rebooting...");
        DEBUG_PRINTLN("Config saved -> Restarting");
        delay(500);
        ESP.restart();
    });

    server.on("/exit", HTTP_GET, [](AsyncWebServerRequest *request) {
        DEBUG_PRINTLN("Exiting");
        request->send(200, "text/plain", "Exiting setup mode...");
        ESP.restart();
    });

    server.begin();
    DEBUG_PRINTLN("AP server started");
}

SystemConfig cfg;

bool loadConfig() {
    if (!prefs.begin("config", true)) {
        prefs.end();
        return false;
    }

    cfg.ssid       = prefs.getString("ssid", "");
    cfg.wifipass   = prefs.getString("wifipass", "");
    cfg.fburl      = prefs.getString("fburl", "");
    cfg.fbapi      = prefs.getString("fbapi", "");
    cfg.email      = prefs.getString("email", "");
    cfg.emailpass  = prefs.getString("emailpass", "");
    cfg.room1      = prefs.getString("room1", "");
    cfg.room2      = prefs.getString("room2", "");
    cfg.unitOrTier = prefs.getString("unitOrTier", "");
    cfg.unitPrice  = prefs.getString("unitPrice", "");

    prefs.end();
    return true;
}

ST7789_extend *tft = new ST7789_extend(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    firebaseUpload = xQueueCreate(10, sizeof(fbData));
    LittleFS.begin();
    DEBUG_PRINTLN("CHECKING CONFIG!");
    tft->init(240, 320);
    tft->invertDisplay(0);
    tft->setRotation(1);
    if (!isConfigSaved()) {
        DEBUG_PRINTLN("Config not found");
        startAP("config", "12345678");
        tft->fillScreen(ST77XX_BLACK);
        tft->print("Waiting for config...", 1, 1, 2, ST77XX_WHITE);
        while(!isConfigSaved()) {
            delay(500);
        }
    }
    else {
        loadConfig();
        DEBUG_PRINTLN(cfg.fburl);
    }
    
    tft->fillScreen(ST77XX_BLACK);
    delete tft;
    tft = nullptr;
    DEBUG("Begin tasks");
    xTaskCreatePinnedToCore(
        TaskNormalOpe,
        "Normal Operation",
        4096,
        NULL,
        6,
        &normalOpe_handle,
        0
    );

    xTaskCreatePinnedToCore(
        TaskWifiCloud,
        "Wifi Cloud",
        10000,
        NULL,
        4,
        &wifiCloud_handle,
        1
    );
}

void loop() {}
