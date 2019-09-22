#include "beelogger_config.h"

//----------------------------------------------------------------
// DS18B20 - Temperatur
//----------------------------------------------------------------
#include <OneWire.h>
#include <DallasTemperature.h>
#define Sensor_Aufloesung 12
//----------------------------------------------------------------


//----------------------------------------------------------------
// DHT21 / DHT22 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#include <dht.h>
//----------------------------------------------------------------


//----------------------------------------------------------------
// Si7021 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#include "Adafruit_Si7021.h"
//----------------------------------------------------------------


//----------------------------------------------------------------
// SHT31 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#include "Adafruit_SHT31.h"
//----------------------------------------------------------------


//----------------------------------------------------------------
// BME280 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#define SEALEVELPRESSURE_HPA (1013.25f)
//----------------------------------------------------------------


//----------------------------------------------------------------
// Beleuchtungsstärke
//----------------------------------------------------------------
#include <Wire.h>
#include <AS_BH1750.h>
//----------------------------------------------------------------

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
}

void hx711_SCK_High() {  // HX711 inaktiv
  digitalWrite (HX711_SCK[0], HIGH);
#if Anzahl_Sensoren_Gewicht > 2
  digitalWrite(HX711_SCK[2], HIGH);
#endif
}

void hx711_SCK_Low() {  // HX711 aktiv
  digitalWrite (HX711_SCK[0], LOW);
#if Anzahl_Sensoren_Gewicht > 2
  digitalWrite(HX711_SCK[2], LOW);
#endif
}


//----------------------------------------------------------------
// Funktion Gewicht
//----------------------------------------------------------------
void Sensor_Gewicht(boolean quick) {

  if ((Anzahl_Sensoren_Gewicht > 0) && (Anzahl_Sensoren_Gewicht < 5)) {

    debugprintlnF(" Waage: ");
    debugflush();

    const float Diff_Gewicht = 0.5;
    HX711 scale;
    for (int i = 0; i < Anzahl_Sensoren_Gewicht; i++) {
      if ((i == 0) || (i == 2)) {
        scale.begin(HX711_DT[i], HX711_SCK[i], 128); // Nr.1,2 Kanal A
      }
      else {
        scale.set_gain(32);  // Nr.1,2 Kanal B
      }
      LowPower.powerStandby(SLEEP_500MS, ADC_OFF, BOD_OFF);  // wait for stable HX711

      for (byte j = 0 ; j < 2; j++) { // Anzahl der Wiederholungen, wenn Abweichung zum letzten Gewicht zu hoch
        long l_gew = scale.read_average(10) - Taragewicht[i];
        Gewicht[i] = (float) l_gew / Skalierung[i];
        if (quick) {
          LetztesGewicht[i] = Gewicht[i];
          break;
        }
        if (fabs(Gewicht[i] - LetztesGewicht[i]) < Diff_Gewicht) break; // Abweichung für Fehlererkennung
        LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF); // Wartezeit zwischen Wiederholungen
      }
      LetztesGewicht[i] = Gewicht[i];

      if ((i == 1) || (i == 3)) {  // 2 oder 4 Waagen
        scale.power_down();
      }
      else if ((i == 0) && (Anzahl_Sensoren_Gewicht == 1)) { // nur 1 Waage
        scale.power_down();
      }
      else if ((i == 2) && (Anzahl_Sensoren_Gewicht == 3)) { // 3 Waagen
        scale.power_down();
      }

      debugprintF(" Gewicht  ");
      debugprint(i + 1);
      debugprintF(" [kg]: ");
      debugprintln(Gewicht[i]);
      debugflush();

    }
  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion DHT21 / DHT22 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
void Sensor_DHT() {
  if ((Anzahl_Sensoren_DHT > 0) and (Anzahl_Sensoren_DHT < 3)) {

    debugprintF(" DHT:  ");
    debugprintln(Anzahl_Sensoren_DHT);
    debugflush();

    float Temperatur_DHT = No_Val;
    float Luftfeuchte_DHT = No_Value;
    int check;
    dht beeDHT;

    for (byte i = 0 ; i < Anzahl_Sensoren_DHT; i++) {
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
      if (i == 1) { // der zweite DHT
        SensorTemp[2] =    Temperatur_DHT;
        SensorFeuchte[2] = Luftfeuchte_DHT;
      }

      debugprintF(" Temperatur  ");
      debugprint(i + 1);
      debugprintF(" [C]: ");
      debugprintln(Temperatur_DHT);
      debugprintF(" Feuchte   ");
      debugprint(i + 1);
      debugprintF(" [%RH]: ");
      debugprintln(Luftfeuchte_DHT);
      debugflush();

    }
  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Si7021 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
void Sensor_Si7021() {
  if (Anzahl_Sensoren_Si7021 == 1) {

    debugprintF(" Si7021: ");
    debugprintln(Anzahl_Sensoren_Si7021);
    debugflush();

    float Temperatur_SI = No_Val;
    float Luftfeuchte_SI = No_Value;

    Adafruit_Si7021 mySI7021 = Adafruit_Si7021();
    int check = mySI7021.begin();

    if (check) {
      Temperatur_SI = mySI7021.readTemperature();
      Luftfeuchte_SI = mySI7021.readHumidity();
    }
    else {
      Temperatur_SI = No_Val;
      Luftfeuchte_SI = No_Value;
    }
    SensorTemp[3] = Temperatur_SI;
    SensorFeuchte[3] = Luftfeuchte_SI;

    debugprintF(" Temperatur [C]: ");
    debugprintln(Temperatur_SI);
    debugprintF(" Feuchte  [%RH]: ");
    debugprintln(Luftfeuchte_SI);
    debugflush();

  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion SHT31 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
void Sensor_SHT31() {

  if ((Anzahl_Sensoren_SHT31 == 1) || (Anzahl_Sensoren_SHT31 == 2)) {

    debugprintF(" SHT31: ");
    debugprintln(Anzahl_Sensoren_SHT31);
    debugflush();

    float Temperatur_SHT31 = No_Val;
    float Luftfeuchte_SHT31 = No_Value;
    boolean check;

    Adafruit_SHT31 my_sht;
    for (byte i = 0 ; i < Anzahl_Sensoren_SHT31; i++) {
      check = my_sht.begin((uint8_t)SHT31_adresse[i]); // I2C Adresse
      delay (100); // time to get system ready
      if (check) { // if SHT ok
        Temperatur_SHT31 = my_sht.readTemperature();
        if (isnan(Temperatur_SHT31)) {
          Temperatur_SHT31 = No_Val;
        }
        else {
          Luftfeuchte_SHT31 = my_sht.readHumidity();
          if (isnan(Luftfeuchte_SHT31)) {
            Luftfeuchte_SHT31 = No_Value;
          }
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
      if (i == 1) { // zweiter SHT31
        SensorTemp[5] = Temperatur_SHT31;
        SensorFeuchte[5] = Luftfeuchte_SHT31;
      }

      debugprintF(" Temperatur  ");
      debugprint(i + 1);
      debugprintF(" [C]: ");
      debugprintln(Temperatur_SHT31);
      debugprintF(" Feuchte   ");
      debugprint(i + 1);
      debugprintF(" [%RH]: ");
      debugprintln(Luftfeuchte_SHT31);
      debugflush();

    }
  }
}
//----------------------------------------------------------------



//----------------------------------------------------------------
// Funktion BME280 - Temperatur und Luftfeuchte
//----------------------------------------------------------------
void Sensor_BME280() {

  if ((Anzahl_Sensoren_BME280 == 1) || (Anzahl_Sensoren_BME280 == 2)) {

    debugprintF(" BME 280: ");
    debugprintln(Anzahl_Sensoren_BME280);
    debugflush();

    float Temperatur_BME = No_Val;
    float Luftfeuchte_BME = No_Value;
    //float Luftdruck_BME = No_Value;
    boolean check;

    Adafruit_BME280 my_bme;
    for (byte i = 0 ; i < Anzahl_Sensoren_BME280; i++) {
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
        else {
          Luftfeuchte_BME = my_bme.readHumidity();
          if (isnan(Luftfeuchte_BME)) {
            Luftfeuchte_BME = No_Value;
          }
          //Luftdruck_BME = my_bme.readPressure();
        }
      }
      else {
        Temperatur_BME = No_Val;
        Luftfeuchte_BME = No_Value;
        //Luftdruck_BME = No_Value;
      }
      if (i == 0) { // erster BME
        SensorTemp[6] = Temperatur_BME;
        SensorFeuchte[6] = Luftfeuchte_BME;
        //L_Druck[0] = Luftdruck_BME;   // Hier kann die Zuordnung der Sensoren geändert werden
      }
      if (i == 1) { // zweiter BME
        SensorTemp[7] = Temperatur_BME;
        SensorFeuchte[7] = Luftfeuchte_BME;
        //L_Druck[1] = Luftdruck_BME;   // Hier kann die Zuordnung der Sensoren geändert werden
      }

      debugprintF(" Temperatur  ");
      debugprint(i + 1);
      debugprintF(" [C]: ");
      debugprintln(Temperatur_BME);
      debugprintF(" Feuchte   ");
      debugprint(i + 1);
      debugprintF(" [%RH]: ");
      debugprintln(Luftfeuchte_BME);
      debugflush();

    }
  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion DS18B20 - Temperatur
//----------------------------------------------------------------
void Sensor_DS18B20() {
  if ((Anzahl_Sensoren_DS18B20 > 0) and (Anzahl_Sensoren_DS18B20 < 3)) {

    debugprintF(" DS18B20: ");
    debugprintln(Anzahl_Sensoren_DS18B20);
    debugflush();

    if (Anzahl_Sensoren_DS18B20 == 1) {   // nur ein DS18B20
      byte i, addr[8], data[12];  // für DS1820
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
      DS_Temp = f_tmp;
      SensorTemp[8] = f_tmp;

      debugprintF(" Temperatur 1 [C]: ");
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

      for (byte i = 0 ; i < sensors.getDeviceCount(); i++) {
        if (sensors.getAddress(Sensor_Adressen, i)) {
          sensors.setResolution(Sensor_Adressen, Sensor_Aufloesung);
        }
      }

      sensors.requestTemperatures();

      for (byte i = 0 ; i < Anzahl_Sensoren_DS18B20; i++) {
        if (i < sensors.getDeviceCount()) {
          Temperatur_DS = sensors.getTempCByIndex(i);
          if (Temperatur_DS == DEVICE_DISCONNECTED_C) {
            Temperatur_DS = No_Val;
          }
        }
        // der erste DS18B20
        if (i == 0) {
          DS_Temp = Temperatur_DS;
          SensorTemp[8] = Temperatur_DS;
        }
        // der zweite DS18B20
        if (i == 1) {
          SensorTemp[9] = Temperatur_DS;
        }

        debugprintF(" Temperatur ");
        debugprint(i + 1);
        debugprintF(" [C]: ");
        debugprintln(Temperatur_DS);
        debugflush();
  
      }
    }
  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Beleuchtungsstärke
//----------------------------------------------------------------
void Sensor_Licht() {

  if (Anzahl_Sensoren_Licht == 1) {

    debugprintF(" BH1750: ");
    debugprintln(Anzahl_Sensoren_Licht);
    debugflush();

    AS_BH1750 sensor;
    if (sensor.begin()) {
      Licht = sensor.readLightLevel();
      sensor.powerDown();
    }

    debugprintF(" Licht [lux]: ");
    debugprintln(Licht);
    debugflush();

  }
}
//----------------------------------------------------------------
