#ifndef CONFIG_H
#define CONFIG_H

//Serial debug
#define SERIAL_DEBUG 1

#if SERIAL_DEBUG
  #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
  #define DEBUG_PRINTF(...)
#endif

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15         
#define TFT_DC    2  
#define TFT_RST   4       

#define VOLT "Voltage: "
#define CURR "Current: "
#define POWR "Power: "
#define ENGY "Energy: "
#define FRQ  "Frequency: "
#define PF   "PF: "

//pzem
#define PZEM0_RX_PIN 16
#define PZEM0_TX_PIN 17
#define PZEM0_SERIAL Serial2

#define PZEM1_RX_PIN 13
#define PZEM1_TX_PIN 14
#define PZEM1_SERIAL Serial1

//RTC DS1307
#define SDA_PIN 27
#define SCL_PIN 26

//NTP
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 7 * 3600
#define DAYLIGHTOFFSET_SEC 0

//unit price
#define UNIT_PRICE 3500

#endif