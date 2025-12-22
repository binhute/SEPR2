#include <Task_bluetooth.h>

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

void TaskBluetooth(void *pvParameters) {
    DEBUG_PRINTLN("Task Bluetooth");
    BluetoothSerial SerialBT;
    String ID = "IDS:" + String(ID1) + "|" + cfg.room1 + "," + String(ID2) + "|" + cfg.room2;
    DEBUG_PRINTLN(ID);
    SerialBT.begin("PMS-2025");
    for (;;) {
        if (SerialBT.hasClient()) {
            if (SerialBT.available()) {
                String msg = SerialBT.readStringUntil('\n');
                msg.trim();

                DEBUG_PRINTLN(msg);
                
                if (msg == "REQUIRE_ID") {
                    SerialBT.println(ID);
                }

                if (msg == "ACK_" + String(ID1) || msg == "ACK_" + String(ID2)) {
                    SerialBT.println("READY");
                }
                
                if (msg.startsWith("WIFI:")) {
                    wssid = parseSSID(msg);
                    wpassword = parsePassword(msg);
                    Serial.println(wssid);
                    Serial.println(wpassword);
                    SerialBT.println("WIFI_OK");
                    break;
                }
            }
        }
    }

    esp_bt_controller_disable();

    prefs.begin("wifi", false);
    prefs.putString("ssid", wssid);
    prefs.putString("password", wpassword);
    prefs.end();

    xTaskCreatePinnedToCore(
        TaskWifiCloud,
        "Wifi Cloud",
        10000,
        NULL,
        4,
        &wifiCloud_handle,
        1
    );

    vTaskDelete(NULL);
}

