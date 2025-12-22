#include <Task_wifi_cloud.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//firebase object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
//Time stamp
String ts;
String ds;

void TaskWifiCloud(void *pvParameters) {
    DEBUG_PRINTLN("Task Wifi");
    WiFi.begin(wssid, wpassword);
    config.api_key = cfg.fbapi;
    config.database_url = cfg.fburl;
    auth.user.email = cfg.email;
    auth.user.password = cfg.emailpass;
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
            // DEBUG_PRINTLN();
            // DEBUG_PRINTLN(fbDataReceive.timeStamp);
            // DEBUG_PRINTLN(fbDataReceive.cost_1);
            // DEBUG_PRINTLN(fbDataReceive.cost_2);
            // DEBUG_PRINTLN(fbDataReceive.energy_1);
            // DEBUG_PRINTLN(fbDataReceive.energy_2);
            
            ds = String(fbDataReceive.dayStamp);
            ts = String(fbDataReceive.timeStamp);
            json.set(ds + "/" + ts, fbDataReceive.energy_1);
            json.set("/Voltage", fbDataReceive.voltage_1);
            Firebase.RTDB.updateNode(&fbdo, "/" + String(ID1), &json);

            json.set(ds + "/" + ts, fbDataReceive.energy_2);
            json.set("/Voltage", fbDataReceive.voltage_2);
            Firebase.RTDB.updateNode(&fbdo, "/" + String(ID2), &json);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
