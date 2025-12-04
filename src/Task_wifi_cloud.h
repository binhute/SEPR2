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

extern Preferences prefs;
extern SystemConfig cfg;
extern QueueHandle_t firebaseUpload;

void TaskWifiCloud(void *pvParameters);
