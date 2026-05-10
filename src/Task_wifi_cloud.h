#ifndef TASK_WIFI_H
#define TASK_WIFI_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <string>
#include <WiFi.h>
#include <Preferences.h>
#include <WiFiUdp.h>
#include "time.h"
#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Firebase.h>
#include <BluetoothSerial.h>
#include "esp_bt.h"

extern Preferences prefs;
extern SystemConfig cfg;
extern QueueHandle_t firebaseUpload;
extern String wssid, wpassword;
extern EventGroupHandle_t sysEvent;

void TaskWifiCloud(void *pvParameters);

#endif