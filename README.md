# ⚡ Smart Energy Monitoring System (Dual Channel)

## 📘 Overview
The **Smart Energy Monitoring System** is an IoT-based project using **ESP32** to measure and monitor two independent electrical loads in real time.  
Each load is monitored by a dedicated **PZEM-004T** energy meter.  
The data (voltage, current, power, and energy) is displayed on a **TFT ST7789** screen and uploaded to **Firebase Realtime Database** for cloud storage.

The system also uses a **DS1307 RTC** to keep time and log data even when offline.

---

## 🚀 Features
- Dual-channel measurement using **2x PZEM-004T** sensors.  
- Real-time data display on **TFT ST7789 (SPI)**.  
- Automatic upload to **Firebase Realtime Database**.  
- Offline data logging with **RTC DS1307**.  
- Auto Wi-Fi reconnect every hour using `millis()`.  
- Flash memory for storing unsent data.  

---

## 🧩 Hardware Components

| Component | Description |
|------------|-------------|
| **ESP32 Dev Board** | Main MCU with Wi-Fi connectivity |
| **PZEM-004T #1** | Energy meter for Load 1 (UART2) |
| **PZEM-004T #2** | Energy meter for Load 2 (UART1) |
| **TFT ST7789** | SPI display for real-time visualization |
| **DS1307 RTC** | Real-Time Clock for timestamps |
| **Wi-Fi network** | For Firebase connectivity |

---

## ⚙️ Typical Wiring (ESP32)

| Device | ESP32 Pins | Notes |
|--------|-------------|--------|
| **PZEM-004T #1** | RX → GPIO 16, TX → GPIO 17 (Serial2) | Measures Line 1 |
| **PZEM-004T #2** | RX → GPIO 13, TX → GPIO 14 (Serial1) | Measures Line 2 |
| **TFT ST7789** | MOSI → GPIO 23, SCK → GPIO 18, CS → GPIO 5, DC → GPIO 2, RST → GPIO 4 |SPI TFT Display |
| **DS1307 RTC** | SDA → GPIO 27, SCL → GPIO 26 | Real-time clock |
| **WiFi / Firebase** | Credentials in `secret.h` | Required for cloud sync |

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

## 🧠 Software Requirements
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


---

## 🔐 Setting Up `secret.h`

Create a file named **`secret.h`** inside the **`src/`** folder with your private credentials:

```cpp
// secret.h
#ifndef SECRET_H
#define SECRET_H

// Wi-Fi Credentials
#define WIFI_SSID "Your_WiFi_Name"
#define WIFI_PASSWORD "Your_WiFi_Password"

// Device ID (unique for each Pzem sensor)
#define ID1 "KH00001"
#define ID2 "KH00002"

// Firebase Configuration
#define API_KEY "Your_Firebase_API_Key"
#define DATABASE_URL "https://your-project-id.firebaseio.com/"
#define USER_EMAIL "your_email@example.com"
#define USER_PASSWORD "your_password"

#endif
```

> ⚠️ **WARNING:** Never upload `secret.h` to any public repository — it contains your private WiFi and Firebase credentials!



