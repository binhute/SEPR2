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
//Bluetooth Classic
extern BluetoothSerial SerialBT;

extern mode BtMode;

extern String wssid, wpassword;

// void changeBtName(const char* newName) {
//     SerialBT.end();
//     delay(200);
//     SerialBT.begin(newName);
// }

String parseSSID(String msg) {
    String data = msg.substring(5);
    int sep = data.indexOf('|');
    if (sep < 0) return "";

    return data.substring(0, sep);
}

String parsePassword(String msg) {
    String data = msg.substring(5);   
    int sep = data.indexOf('|');
    if (sep < 0) return "";

    return data.substring(sep + 1);
}

void TaskWifiCloud(void *pvParameters) {
    config.api_key = cfg.fbapi;
    config.database_url = cfg.fburl;
    auth.user.email = cfg.email;
    auth.user.password = cfg.emailpass;
    config.token_status_callback = tokenStatusCallback;
    fbData fbDataReceive;
    if (BtMode != mode::OFF) {
        bool idSent = false;
        bool ackReceived = false;
        String ID = String(BtMode == mode::device_1? ID1: ID2);
        DEBUG_PRINTLN("Start Bluetooth");
        SerialBT.begin("PMS-" + ID);
        for (;;) {
            if (SerialBT.hasClient()) {
                if (!idSent) {
                    SerialBT.println(ID);
                    idSent = true;
                }
                if (SerialBT.available()) {
                    String msg = SerialBT.readStringUntil('\n');
                    msg.trim();

                    if (msg == "ACK_" + ID) {
                        ackReceived = true;
                        SerialBT.println("READY");
                    }
                    
                    if (msg.startsWith("WIFI:")) {
                        wssid = parseSSID(msg);
                        wpassword = parsePassword(msg);
                        Serial.println(wssid);
                        Serial.println(wpassword);
                        WiFi.begin(wssid, wpassword);
                        SerialBT.println("WIFI_OK");
                        break;
                    }
                }
            } else {
                idSent = false;
                ackReceived = false;
            }
        }
    }
    SerialBT.end(); 
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
            json.set(ds + "/" + ts + "/Cost", fbDataReceive.cost_1);
            json.set(ds + "/" + ts + "/Energy", fbDataReceive.energy_1);
            json.set("/Voltage", fbDataReceive.voltage_1);
            Firebase.RTDB.updateNode(&fbdo, "/" + String(ID1), &json);

            json.set(ds + "/" + ts + "/Cost", fbDataReceive.cost_2);
            json.set(ds + "/" + ts + "/Energy", fbDataReceive.energy_2);
            json.set("/Voltage", fbDataReceive.voltage_2);
            Firebase.RTDB.updateNode(&fbdo, "/" + String(ID2), &json);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
