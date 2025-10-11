#ifndef CONFIG_H
#define CONFIG_H

//TFT
// #define TFT_CS    27
// #define TFT_DC    32
// #define TFT_RST   14
// #define TFT_SCLK  25
// #define TFT_MOSI  33
// #define TFT_MISO  26

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

//unit price
#define UNIT_PRICE 3500

#endif