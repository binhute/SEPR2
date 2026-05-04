# Smart Energy Monitoring System (Dual Channel)

## Overview
The **Smart Energy Monitoring System** is an RTOS-based IoT project using **ESP32** to measure and monitor two independent electrical loads in real time.  
Each load is monitored by a dedicated **PZEM-004T** energy meter.  

The collected data — including voltage, current, power, and energy — is displayed locally on a **TFT ST7789** screen and uploaded to **Firebase Realtime Database** for cloud storage.  

The system uses a **DS1307 RTC** to maintain accurate timestamps and log data even when offline.  

Device configuration (Wi-Fi, Firebase credentials) is handled via a **web page stored in LittleFS**, allowing users to set or update parameters directly through a browser without recompiling the code.

---

## Features
- Dual-channel measurement using **2x PZEM-004T** sensors.  
- Real-time data display on **TFT ST7789 (SPI)**.  
- Automatic upload to **Firebase Realtime Database**.  
- Offline data logging with **RTC DS1307**.  
- Auto Wi-Fi reconnect every hour using `millis()`.  
- Flash memory for storing unsent data.  
- RTOS-based system with web configuration via LittleFS for Wi-Fi, Firebase config.

---

## Hardware Components

| Component | Description |
|------------|-------------|
| **ESP32 Dev Board** | Main MCU with Wi-Fi connectivity |
| **PZEM-004T #1** | Energy meter for Load 1 (UART2) |
| **PZEM-004T #2** | Energy meter for Load 2 (UART1) |
| **TFT ST7789** | SPI display for real-time visualization |
| **DS1307 RTC** | Real-Time Clock for timestamps |
| **Wi-Fi network** | For Firebase connectivity |

---

## Typical Wiring (ESP32)

| Device | ESP32 Pins | Notes |
|--------|-------------|--------|
| **PZEM-004T #1** | RX → GPIO 16, TX → GPIO 17 (Serial2) | Measures Line 1 |
| **PZEM-004T #2** | RX → GPIO 13, TX → GPIO 14 (Serial1) | Measures Line 2 |
| **TFT ST7789** | MOSI → GPIO 23, SCK → GPIO 18, CS → GPIO 5, DC → GPIO 2, RST → GPIO 4 |SPI TFT Display |
| **DS1307 RTC** | SDA → GPIO 27, SCL → GPIO 26 | Real-time clock |

### Example Pin Definitions:
```cpp
// PZEM Sensors
#define PZEM0_RX_PIN 16
#define PZEM0_TX_PIN 17
#define PZEM0_SERIAL Serial2

#define PZEM1_RX_PIN 13
#define PZEM1_TX_PIN 14
#define PZEM1_SERIAL Serial1
```

---

## Software Requirements
- **VS Code** with **PlatformIO** extension (recommended)
- Required Libraries:

| Library | Description | Source / Link |
|----------|--------------|----------------|
| **Firebase ESP Client** | Firebase Realtime Database client for ESP32 | [GitHub – mobizt/Firebase-ESP-Client](https://github.com/mobizt/Firebase-ESP-Client) |
| **PZEM004Tv30** | Interface for PZEM-004T v3.0 power monitor | [GitHub – mandulaj/PZEM-004T-v30](https://github.com/mandulaj/PZEM-004T-v30) |
| **Adafruit ST7789** | Driver for ST7789 SPI TFT display | [GitHub – adafruit/Adafruit-ST7789-Library](https://github.com/adafruit/Adafruit-ST7789-Library) |
| **Adafruit GFX Library** | Core graphics library for TFT displays | [GitHub – adafruit/Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) |
| **RTClib** | Library for DS1307/DS3231 RTC modules | [GitHub – adafruit/RTClib](https://github.com/adafruit/RTClib) |
| **ArduinoJson** | JSON handling for Firebase requests | [GitHub – bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson) |
| **ESPAsyncWebServer** | Asynchronous web server for ESP32/ESP8266, used to serve configuration pages and handle HTTP requests | [GitHub – me-no-dev/ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) |


---

## Configuration via Web Page (LittleFS)

This project uses a web-based configuration page stored in LittleFS instead of compile-time constants.  
Users can enter their Wi-Fi, Firebase information, and other parameters directly from a browser.  
All settings are saved to flash and automatically loaded at startup.

### Steps to Upload Configuration Page
1. Place your configuration file (`index.html` and `style.css`) inside:
   `data/`

2. Upload the file to LittleFS using PlatformIO:
   - VS Code → PlatformIO → "Upload Filesystem Image"
   - Or CLI:
        ```shell
        pio run --target uploadfs
        ```

3. Reboot the ESP32.  
   The device will create an Access Point (AP) and host the configuration webpage.

4. Connect to the AP (SSID: `config`, password: `12345678` or change its in your code) → open a browser → fill in Wi-Fi & Firebase configuration values.  
   The ESP32 saves them to flash and reconnects automatically.

> WARNING:  
> Credentials are stored inside the device.  
> Do **not** upload filesystem images or flash dumps to public repositories.


