#ifndef __beelogger_config__
#define __beelogger_config__
// 09.03.2020
// 26.03.2020 Konfiguration Regensensor
// 02.04.2020  HEXA, 6 Waagen
// 10.05.2020  Senden bei Änderung Gewicht
//----------------------------------------------------------------
// Konfiguration DEBUG-Information
//               Serial Monitor via FTDI / USB
//----------------------------------------------------------------
#define myDEBUG 1     // Debug via Serial Monitor
//----------------------------------------------------------------
// Konfig der seriellen Schnittstelle
//----------------------------------------------------------------
#define Serial_Baudrate 9600
//----------------------------------------------------------------

//----------------------------------------------------------------
// Änderung Gewicht in Kilogramm bei der Daten versenden erfolgt
//----------------------------------------------------------------
const float Alarm_Gewicht = 1.0;
//----------------------------------------------------------------

//################################################################
// I2C Adressen
//################################################################

//----------------------------------------------------------------
// Datenablage EEProm 
// #define EE_PROM_USE 1     // aktiveren
// #define EE_PROM_USE 0     // nicht aktiv
//----------------------------------------------------------------
#define EE_PROM_USE 0

#define AT24Cxx_CTRL_ID 0x57  // EE-Prom I2C Adresse

#include <beelogger_at24cxx.h>
AT24Cxx my_EEPROM(AT24Cxx_CTRL_ID);
//----------------------------------------------------------------

//----------------------------------------------------------------
// Konfiguration SHT31 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
uint8_t SHT31_adresse[2] = {0x44, 0x45};
//----------------------------------------------------------------


//----------------------------------------------------------------
// Konfiguration BME280 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#define BME280_1_adresse (0x76)
#define BME280_2_adresse (0x77)
//----------------------------------------------------------------


//----------------------------------------------------------------
// Konfiguration Si7021 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
// Auswahl der Library
#define SI7021_Type 1  // 0 = Adafruit Lib, 1 = SI7021 Lib Marcus Sorensen
// Si7021 I2C-Adresse : 0x40 (64d), Wert ist in Adafruit_Si7021.h
//----------------------------------------------------------------


//################################################################
// AT-Mega Pins
//################################################################

//----------------------------------------------------------------
// Konfiguration One-Wire-Bus für DS18B20
// Regenmesser verwendet Pin 3, dann DS18B20 auf Pin 6
//----------------------------------------------------------------
#if  Anzahl_Sensor_Regen
#define ONE_WIRE_BUS 6
#else
#define ONE_WIRE_BUS 3
#endif
//----------------------------------------------------------------

//----------------------------------------------------------------
// Konfiguration DHT21 / DHT22 - Temperatur und Luftfeuchte
// mit Regenmesser verwendet DS18B20 Pin 6
//----------------------------------------------------------------
#if  Anzahl_Sensor_Regen
uint8_t DHT_Sensor_Pin[2] = {5, 5}; // nur ein DHT
#else
uint8_t DHT_Sensor_Pin[2] = {5, 6}; // zwei DHT
#endif
//----------------------------------------------------------------

//----------------------------------------------------------------
// Konfiguration HX711 SCK /Data
//----------------------------------------------------------------
uint8_t HX711_SCK[6] = {A0, A0, 10, 10, 6 ,6}; // HX711 Nr.1: A,B;  HX711 Nr.2: A,B  HX711 Nr.3: A,B S-Clock
uint8_t HX711_DT[6] =  {A1, A1, 11, 11, 7, 7}; // HX711 Nr.1: A,B;  HX711 Nr.2: A,B  HX711 Nr.3: A,B Data
//----------------------------------------------------------------

//----------------------------------------------------------------
// Konfiguration ESP / SIM800
//----------------------------------------------------------------
// Pin Belegung ESP8266
#define ESP_RESET A2
byte ESP_RX = 8;
byte ESP_TX = 9;

// Pin  Belegung SIM 800
#define GSM_Power_Pin  A2
byte GSM_TX = 9;
//byte GSM_RX = 8;    // beelogger_Universal (NANO), solar Multishield 2.5, neue Konfiguration
byte GSM_RX = 10; // beelogger Solar (PRO MINI), alte Konfiguration

//----------------------------------------------------------------

//----------------------------------------------------------------
// Seriellen Schnittstelle
//----------------------------------------------------------------
#define USB_RX      0          // Pin 0 RX-USB
#define USB_TX      1          // Pin 1 TX-USB
//----------------------------------------------------------------

//----------------------------------------------------------------
// Interrupt
//----------------------------------------------------------------
#define DS3231_Interrupt_Pin 2
//----------------------------------------------------------------

//----------------------------------------------------------------
// Power on/off
//----------------------------------------------------------------
#define Power_Pin 4
//----------------------------------------------------------------

//----------------------------------------------------------------
// I2C
//----------------------------------------------------------------
#define SDA   A4      // I2C Daten
#define SCL   A5      // I2C Clock
//----------------------------------------------------------------

//----------------------------------------------------------------
// Pins Spannungen messen
//----------------------------------------------------------------
#define Batterie_messen    A6
#define Solarzelle_messen  A7
//----------------------------------------------------------------


//################################################################
// Global Libraries
//################################################################
#include <Sodaq_DS3231.h>
#include <LowPower.h>
//----------------------------------------------------------------

//################################################################
// Debug Print Functions
//################################################################

#if myDEBUG == 1
#define debugbegin(x) Serial.begin(x)
#define debugprintF(x) Serial.print(F(x))
#define debugprintlnF(x) Serial.println(F(x))
#define debugprint(x) Serial.print(x)
#define debugprintH(x) Serial.print(x,HEX)
#define debugprintln(x) Serial.println(x)
#define debugflush() Serial.flush()
#define debugend() Serial.end()
#else
#define debugbegin(x)
#define debugprintF(x)
#define debugprintlnF(x)
#define debugprint(x)
#define debugprintH(x)
#define debugprintln(x)
#define debugflush()
#define debugend()
#endif

void Serial_rxtx_off() {
  delay(20);
  digitalWrite(USB_RX, LOW);   // Port aus
  pinMode(USB_TX, INPUT);
  digitalWrite(USB_TX, LOW);   // Port aus
}
//----------------------------------------------------------------
#endif // beelogger_config.h
