#include "beelogger_config.h"
// 08.02.2020
// 02.04.2020  HEXA, 6 Waagen
// 08.04.2020  delay in DS18B20
// 15.04.2020  4x DS18B20
// 10.05.2020  Senden bei Änderung Gewicht
// 03.07.2020  Korrektur Luftdruck auf Standort
// 18.07.2020  ignoriere Waagen ohne Kal-Werte
// 02.09.2020  Waagen ohne Kal-Werte
// 25.10.2020  Temperatur Waegezelle
//----------------------------------------------------------------
// HX711 utility functions
//----------------------------------------------------------------
#include <HX711.h>
void setup_hx711() {  // HX711 inaktiv
  hx711_SCK_High();
  pinMode(HX711_SCK[0], OUTPUT);
#if Anzahl_Sensoren_Gewicht > 2
  pinMode(HX711_SCK[2], OUTPUT);
#endif
#if Anzahl_Sensoren_Gewicht > 4
  pinMode(HX711_SCK[4], OUTPUT);
#endif
}

void hx711_SCK_High() {  // HX711 inaktiv
  digitalWrite (HX711_SCK[0], HIGH);
#if Anzahl_Sensoren_Gewicht > 2
  digitalWrite(HX711_SCK[2], HIGH);
#endif
#if Anzahl_Sensoren_Gewicht > 4
  digitalWrite(HX711_SCK[4], HIGH);
#endif
}

void hx711_SCK_Low() {  // HX711 aktiv
  digitalWrite (HX711_SCK[0], LOW);
#if Anzahl_Sensoren_Gewicht > 2
  digitalWrite(HX711_SCK[2], LOW);
#endif
#if Anzahl_Sensoren_Gewicht > 4
  digitalWrite(HX711_SCK[4], LOW);
#endif
}

//----------------------------------------------------------------
// Funktion Temperatur Wägezelle
//----------------------------------------------------------------
void Sensor_Temp_Zelle(boolean set) {
  if (set) {
    DS_Temp = SensorTemp[Temp_Zelle];    // duo, tripple, quad, ...
    if (DS_Temp == No_Val) {
      DS_Temp = rtc.getTemperature();    // force
    }
    if (SensorTemp[Aussenwerte] == No_Val) { // single, force
      SensorTemp[Aussenwerte] = DS_Temp;
    }
  }
  else {
    DS_Temp = No_Val;
    SensorTemp[Aussenwerte] = No_Val;
    Aux[3] = rtc.getTemperature();     // default Aux[3]
  }
}

//----------------------------------------------------------------
// Funktion Gewicht
//----------------------------------------------------------------
uint8_t Sensor_Gewicht(boolean quick) {
  uint8_t ret_val = 0;
  if ((Anzahl_Sensoren_Gewicht > 0) && (Anzahl_Sensoren_Gewicht < 7)) {

    const float Diff_Gewicht = 0.5;
    HX711 scale;
    for (uint8_t i = 0; i < Anzahl_Sensoren_Gewicht; i++) {
      if ((i == 0) || (i == 2) || (i == 4)) {
        scale.begin(HX711_DT[i], HX711_SCK[i], 128); // Nr.1,2 Kanal A
      }
      else {
        scale.set_gain(32);  // Nr.1,2 Kanal B
      }
      if (Taragewicht[i] != 10) {
        LowPower.powerStandby(SLEEP_500MS, ADC_OFF, BOD_OFF);  // wait for stable HX711

        for (uint8_t j = 0 ; j < 2; j++) { // Anzahl der Wiederholungen, wenn Abweichung zum letzten Gewicht zu hoch
          long l_gew = scale.read_average(10) - Taragewicht[i];
          Gewicht[i] = ((float) l_gew) / Skalierung[i];
          if (quick) {
            LetztesGewicht[i] = Gewicht[i];
            break;
          }
          if (fabs(Gewicht[i] - LetztesGewicht[i]) < Diff_Gewicht) break; // Abweichung für Fehlererkennung
          LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF); // Wartezeit zwischen Wiederholungen
        }
        if (fabs(Gewicht[i] - LetztesGewicht[i]) > Alarm_Gewicht) ret_val = 1;

        LetztesGewicht[i] = Gewicht[i];
      }

      if ((i == 1) || (i == 3) || (i == 5)) { // 2, 4 oder 6 Waagen
        scale.power_down();
      }
      else if ((i == 0) && (Anzahl_Sensoren_Gewicht == 1)) { // nur 1 Waage
        scale.power_down();
      }
      else if ((i == 2) && (Anzahl_Sensoren_Gewicht == 3)) { // 3 Waagen
        scale.power_down();
      }
      else if ((i == 4) && (Anzahl_Sensoren_Gewicht == 5)) { // 5 Waagen
        scale.power_down();
      }

      debugprintF(" W ");
      debugprint(i + 1);
      debugprintF(" [kg]: ");
      debugprintln(Gewicht[i]);
      debugflush();

    }
  }
  return (ret_val);
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion DHT21 / DHT22 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#if ((Anzahl_Sensoren_DHT > 0) && (Anzahl_Sensoren_DHT < 3))

#include <dht.h>

void Sensor_DHT() {

  float Temperatur_DHT = No_Val;
  float Luftfeuchte_DHT = No_Value;
  int check;
  dht beeDHT;

  for (uint8_t i = 0 ; i < Anzahl_Sensoren_DHT; i++) {
    LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF); //Wartezeit

    check = beeDHT.read(DHT_Sensor_Pin[i]);

    if (check == DHTLIB_OK) {
      Luftfeuchte_DHT = beeDHT.humidity;
      Temperatur_DHT = beeDHT.temperature;
    }
    else {
      Temperatur_DHT = No_Val;
      Luftfeuchte_DHT = No_Value;
    }
    if (i == 0) { // der erste DHT
      SensorTemp[1] =    Temperatur_DHT;
      SensorFeuchte[1] = Luftfeuchte_DHT;
    }
    else { // der zweite DHT
      SensorTemp[2] =    Temperatur_DHT;
      SensorFeuchte[2] = Luftfeuchte_DHT;
    }

    debugprintlnF("DHT:");
    debugprintF(" [C]: ");
    debugprintln(Temperatur_DHT);
    debugprintF(" [%RH]: ");
    debugprintln(Luftfeuchte_DHT);
    debugflush();

  }
}
#else
void Sensor_DHT() {}
#endif
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Si7021 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#if (Anzahl_Sensoren_Si7021 == 1)

#if SI7021_Type == 1
#include "SI7021.h"
#else
#include "Adafruit_Si7021.h"
#endif

void Sensor_Si7021() {

  float Temperatur_SI = No_Val;
  float Luftfeuchte_SI = No_Value;

#if SI7021_Type
  SI7021 sensor7021;
  int check = sensor7021.begin();
#else
  Adafruit_Si7021 mySI7021 = Adafruit_Si7021();
  int check = mySI7021.begin();
#endif

  if (check) {
#if SI7021_Type
    int temperature = sensor7021.getCelsiusHundredths();
    Temperatur_SI = ((float) temperature) / 100.0;
    int humidity = sensor7021.getHumidityPercent();
    Luftfeuchte_SI = ((float) humidity );
#else
    Temperatur_SI = mySI7021.readTemperature();
    Luftfeuchte_SI = mySI7021.readHumidity();
#endif
  }
  else {
    Temperatur_SI = No_Val;
    Luftfeuchte_SI = No_Value;
  }
  SensorTemp[3] = Temperatur_SI;
  SensorFeuchte[3] = Luftfeuchte_SI;

  debugprintlnF("Si7021:");
  debugprintF(" [C]: ");
  debugprintln(Temperatur_SI);
  debugprintF(" [%RH]: ");
  debugprintln(Luftfeuchte_SI);
  debugflush();

}
#else
void Sensor_Si7021() {};
#endif
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion SHT31 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#if ((Anzahl_Sensoren_SHT31 == 1) || (Anzahl_Sensoren_SHT31 == 2))

#include "Adafruit_SHT31.h"

void Sensor_SHT31() {

  float Temperatur_SHT31 = No_Val;
  float Luftfeuchte_SHT31 = No_Value;
  boolean check;

  Adafruit_SHT31 my_sht;
  for (uint8_t i = 0 ; i < Anzahl_Sensoren_SHT31; i++) {
    check = my_sht.begin((uint8_t)SHT31_adresse[i]); // I2C Adresse
    delay (100); // time to get system ready
    if (check) { // if SHT ok
      Temperatur_SHT31 = my_sht.readTemperature();
      if (isnan(Temperatur_SHT31)) {
        Temperatur_SHT31 = No_Val;
      }
      Luftfeuchte_SHT31 = my_sht.readHumidity();
      if (isnan(Luftfeuchte_SHT31)) {
        Luftfeuchte_SHT31 = No_Value;
      }
    }
    else {
      Temperatur_SHT31 = No_Val;
      Luftfeuchte_SHT31 = No_Value;
    }
    if (i == 0) { // erster SHT31
      SensorTemp[4] = Temperatur_SHT31;
      SensorFeuchte[4] = Luftfeuchte_SHT31;
    }
    else { // zweiter SHT31
      SensorTemp[5] = Temperatur_SHT31;
      SensorFeuchte[5] = Luftfeuchte_SHT31;
    }

    debugprintlnF("SHT31:");
    debugprintF(" [C]: ");
    debugprintln(Temperatur_SHT31);
    debugprintF(" [%RH]: ");
    debugprintln(Luftfeuchte_SHT31);
    debugflush();

  }
}
#else
void Sensor_SHT31() {};
#endif
//----------------------------------------------------------------



//----------------------------------------------------------------
// Funktion BME280 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#if ((Anzahl_Sensoren_BME280 == 1) || (Anzahl_Sensoren_BME280 == 2))

#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#define SEALEVELPRESSURE_HPA (1013.25f)

void Sensor_BME280() {

  float Temperatur_BME = No_Val;
  float Luftfeuchte_BME = No_Value;
  float Luftdruck_BME = No_Value;
  boolean check;

  Adafruit_BME280 my_bme;
  for (uint8_t i = 0 ; i < Anzahl_Sensoren_BME280; i++) {
    if ( i == 0) {
      check = my_bme.begin((uint8_t)BME280_1_adresse); // I2C Adresse
    }
    else {
      check = my_bme.begin((uint8_t)BME280_2_adresse); // I2C Adresse
    }
    delay (100); // time to get system ready
    if (check) { // if bme ok
      Temperatur_BME = my_bme.readTemperature();
      if (isnan(Temperatur_BME)) {
        Temperatur_BME = No_Val;
      }
      Luftfeuchte_BME = my_bme.readHumidity();
      if (isnan(Luftfeuchte_BME)) {
        Luftfeuchte_BME = No_Value;
      }
      Luftdruck_BME = my_bme.readPressure();
      if (isnan(Luftdruck_BME)) {
        Luftdruck_BME = No_Value;
      }
      else {
        Luftdruck_BME = Luftdruck_BME / 100.0;
        Luftdruck_BME  = Luftdruck_BME + Hoehe_Standort / 8.0;
      }
    }
    else {
      Temperatur_BME = No_Val;
      Luftfeuchte_BME = No_Value;
      Luftdruck_BME = No_Value;
    }
    debugprintlnF("BME280:");
    if (i == 0) { // erster BME
      SensorTemp[6] = Temperatur_BME;
      SensorFeuchte[6] = Luftfeuchte_BME;

      Aux[1] = Luftdruck_BME;              // Hier kann die Zuordnung der Sensoren geändert werden
      debugprintF(" [hPa]: ");
      debugprintln(Luftdruck_BME);
    }
    else { // zweiter BME
      SensorTemp[7] = Temperatur_BME;
      SensorFeuchte[7] = Luftfeuchte_BME;
      // Aux[1] = Luftdruck_BME;             // Hier kann die Zuordnung der Sensoren geändert werden
    }

    debugprintF(" [C]: ");
    debugprintln(Temperatur_BME);
    debugprintF(" [%RH]: ");
    debugprintln(Luftfeuchte_BME);
    debugflush();

  }
}
#else
void Sensor_BME280() {};
#endif
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion DS18B20 - Temperatur
//----------------------------------------------------------------
#if ((Anzahl_Sensoren_DS18B20 > 0) && (Anzahl_Sensoren_DS18B20 < 5))

#include <OneWire.h>
#include <DallasTemperature.h>
#define Sensor_Aufloesung 12

void Sensor_DS18B20() {

  debugprintlnF("DS18B20:");

  if (Anzahl_Sensoren_DS18B20 == 1) {   // nur ein DS18B20
    uint8_t i, addr[8], data[12];  // für DS1820
    float f_tmp  = No_Val;
    OneWire ds1820(ONE_WIRE_BUS);
    if (ds1820.search(addr)) {
      if (OneWire::crc8( addr, 7) == addr[7]) { // CRC OK
        if (addr[0] == 0x28) {   // teste auf DS18_B_20
          ds1820.reset();
          ds1820.select(addr);
          ds1820.write(0x44, 0);  // start conversion, with parasite power off
          delay(800);             // wait for conversion
          i = ds1820.reset();
          ds1820.select(addr);
          ds1820.write(0xBE);        // Read Scratchpad
          for ( i = 0; i < 3; i++) { // we need only 3 of 9 bytes
            data[i] = ds1820.read();
          }
          f_tmp = (float)((data[1] << 8) + data[0]);  // MSB + LSB
          f_tmp = (f_tmp / 16.0);        // 12Bit = 0,0625 C per Bit (-> teile mit 16)
        }
      }  // CRC OK
    } // if ds18b20
    SensorTemp[8] = f_tmp;

    debugprintF(" [C]: ");
    debugprintln(f_tmp);
    debugflush();

  }
  else {
    // wenn mehr als ein DS18B20
    float Temperatur_DS = No_Val;
    DeviceAddress Sensor_Adressen;
    OneWire oneWire(ONE_WIRE_BUS);
    DallasTemperature sensors(&oneWire);

    sensors.begin();

    for (uint8_t i = 0 ; i < sensors.getDeviceCount(); i++) {
      if (sensors.getAddress(Sensor_Adressen, i)) {
        sensors.setResolution(Sensor_Adressen, Sensor_Aufloesung);
      }
    }

    sensors.requestTemperatures();
    delay(250); // add. Wartezeit
    for (uint8_t i = 0 ; i < Anzahl_Sensoren_DS18B20; i++) {
      if (i < sensors.getDeviceCount()) {
        Temperatur_DS = sensors.getTempCByIndex(i);
        if (Temperatur_DS == DEVICE_DISCONNECTED_C) {
          Temperatur_DS = No_Val;
        }
      }

      if (i == 0) { // der erste DS18B20
        SensorTemp[8] = Temperatur_DS;
      }
      if (i == 1) { // der zweite DS18B20
        SensorTemp[9] = Temperatur_DS;
      }
      if (i == 2) { // der dritte DS18B20
        SensorFeuchte[8] = Temperatur_DS;
      }
      if (i == 3) { // der vierte DS18B20
        SensorFeuchte[9] = Temperatur_DS;
      }
      debugprintF(" [C]: ");
      debugprintln(Temperatur_DS);
      debugflush();

    }
  }
}
#else
void Sensor_DS18B20() {};
#endif
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Beleuchtungsstärke
//----------------------------------------------------------------
#if (Anzahl_Sensoren_Licht == 1)

#include <Wire.h>
#include <AS_BH1750.h>

void Sensor_Licht() {

  AS_BH1750 sensor;
  if (sensor.begin()) {
    Licht = sensor.readLightLevel();
    sensor.powerDown();
  }

  debugprintlnF("BH1750:");
  debugprintF(" [lux]: ");
  debugprintln(Licht);
  debugflush();

}
#else
void Sensor_Licht() {};
#endif
//----------------------------------------------------------------
