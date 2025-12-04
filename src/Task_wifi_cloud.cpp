#include <Task_wifi_cloud.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//firebase object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
//RTDB path
String path_1;
String path_2;
void TaskWifiCloud(void *pvParameters) {
    DEBUG_PRINTLN(1);
    unsigned long startAttemptTime = millis();
    DEBUG_PRINTLN(cfg.ssid);
    DEBUG_PRINTLN(cfg.wifipass);
    WiFi.begin(cfg.ssid, cfg.wifipass);
    DEBUG_PRINTLN(WiFi.localIP());
    config.api_key = cfg.fbapi;
    DEBUG_PRINTLN(config.api_key.c_str());
    config.database_url = cfg.fburl;
    DEBUG_PRINTLN(config.database_url.c_str());
    auth.user.email = cfg.email;
    DEBUG_PRINTLN(auth.user.email.c_str());
    auth.user.password = cfg.emailpass;
    DEBUG_PRINTLN(auth.user.password.c_str());
    config.token_status_callback = tokenStatusCallback;
    fbData fbDataReceive;
    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
            DEBUG_PRINTLN("DISCONNECTED");
            WiFi.reconnect();
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            continue;
        }
        if (!Firebase.ready()) {
            Firebase.begin(&config, &auth);
            Firebase.reconnectWiFi(true);
        }
        if (xQueueReceive(firebaseUpload, &fbDataReceive, portMAX_DELAY) == pdPASS) {
            DEBUG_PRINTLN();
            DEBUG_PRINTLN(fbDataReceive.regularTime);
            DEBUG_PRINTLN(fbDataReceive.cost_1);
            DEBUG_PRINTLN(fbDataReceive.cost_2);
            DEBUG_PRINTLN(fbDataReceive.energy_1);
            DEBUG_PRINTLN(fbDataReceive.energy_2);
            
            path_1 = "/" + String(ID1) + "/" + String(fbDataReceive.regularTime) + "/";
            path_2 = "/" + String(ID2) + "/" + String(fbDataReceive.regularTime) + "/";
            if (Firebase.RTDB.setFloat(&fbdo, path_1 + "Energy", fbDataReceive.energy_1)) {
                DEBUG_PRINTLN("Success upload");
            }
            else DEBUG_PRINTLN(fbdo.errorReason());

            if (Firebase.RTDB.setFloat(&fbdo, path_1 + "Cost", fbDataReceive.cost_1)) {
                DEBUG_PRINTLN("Success upload");
            }
            else DEBUG_PRINTLN(fbdo.errorReason());

            if (Firebase.RTDB.setFloat(&fbdo, path_2 + "Energy", fbDataReceive.energy_2)) {
                DEBUG_PRINTLN("Success upload");
            }
            else DEBUG_PRINTLN(fbdo.errorReason());

            if (Firebase.RTDB.setFloat(&fbdo, path_2 + "Cost", fbDataReceive.cost_2)) {
                DEBUG_PRINTLN("Success upload");
            }
            else DEBUG_PRINTLN(fbdo.errorReason());
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
