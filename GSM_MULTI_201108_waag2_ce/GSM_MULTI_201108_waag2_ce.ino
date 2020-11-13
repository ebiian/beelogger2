/*
   (C) 2020 R.Schick / Thorsten Gurzan - beelogger.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// beelogger.de - Arduino Datenlogger für Imker
// Erläuterungen dieses Programmcodes unter http://beelogger.de
// Version 2020    beelogger Multi GSM mit EE-Prom, bis zu sechs Waagen

#include "GSM_MULTI_config.h"     // Konfiguration und Kalibrierung eintragen
#include "beelogger_config.h"


#if EE_PROM_USE
char ID[] = "GSM_MULTI_EE_201108_waag2_ce";  //nur Grossbuchstaben,Zahlen, keine Blank
#else
char ID[] = "GSM_MULTI_201108_waag2_ce";  //nur Grossbuchstaben,Zahlen, keine Blank
#endif

//----------------------------------------------------------------
// Variablen
//----------------------------------------------------------------
const float No_Val = 99.9f;  // Vorbelegung, Wert nicht gemessen
const float No_Value = -1.0f;  // Vorbelegung, Wert nicht gemessen

//                              DHT1,   DHT2,   Si7021, SHT31a, SHT31b, BMEa,   BMEb,  DS18B20a,DS18B20b
float SensorTemp[10] = {No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val};

//                                  DHT1,      DHT2,     Si7021,   SHT31a,   SHT31b,   BMEa,     BMEb    , leer    , leer
float SensorFeuchte[10] = {No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value};

//              Kein_Sensor, Luftdruck, ZählerRegen, unbelegt
float Aux[4] = {No_Value, No_Value, No_Value, No_Value};

float Licht = No_Value;
float DS_Temp = No_Val;

float Gewicht[6] = {No_Value, No_Value, No_Value, No_Value, No_Value, No_Value};
float LetztesGewicht[6] = {No_Value, No_Value, No_Value, No_Value, No_Value, No_Value};

float Batteriespannung = No_Value;
float Solarspannung = No_Value;
float Service = 0.0;
uint32_t time_on = 0;

int WeckIntervallMinuten = 2;   // Testsenden bei Start
DateTime aktuell;

byte report_info = 0;           // 0 = time-On, 1 = Pegel, 2 = Volt, 3 = Location
byte my_counter = 0;            // Anzahl Messungen
byte nr_to_measure = 1;         // Senden bei Start
byte next_send_hour = 0;        //
int send_cycle = 1;             // Normaler Sendezyklus

// Interrupt Variablen
volatile byte ok_sleep = true;
char DatenString[_Daten_Satz_Len];  // Datenbuffer
//----------------------------------------------------------------

void setup() {

  Serial.begin(Serial_Baudrate);
  Serial.println(ID);
  Serial.println();
  Serial.flush();

  //----------------------------------------------------------------
  //  System on
  //----------------------------------------------------------------

  stop_DFUe_device();

  Spannungen_messen();
  while (Batteriespannung < VMinimum) {
    LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_OFF);
    Spannungen_messen();
  }

  digitalWrite(Power_Pin, HIGH);     // Power On
  pinMode(Power_Pin, OUTPUT);
  delay(5);
  setup_hx711();
  //----------------------------------------------------------------


  //----------------------------------------------------------------
  //  Check EE-Prom
  //----------------------------------------------------------------
#if EE_PROM_USE
  if (my_EEPROM.isPresent()) {
    Serial.print(F("EE-PROM fehlt"));
    Serial.flush();
    digitalWrite(Power_Pin, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
  else {
    Serial.print(F("EE-PROM size in use: "));
    Serial.println((_Num_Measured + _Num_Retry) * _Daten_Satz_Len);
    Serial.println(sizeof(DatenString));
    Serial.println();
    Serial.flush();
  }
#endif
  //----------------------------------------------------------------


  //----------------------------------------------------------------
  //  Sleep Mode Interrupt             05.08.2020
  //----------------------------------------------------------------
  pinMode(DS3231_Interrupt_Pin, INPUT_PULLUP);
  delay(5);
  //----------------------------------------------------------------

  System_On();

  //----------------------------------------------------------------
  // Setup Gewicht
  //----------------------------------------------------------------
  Sensor_Gewicht(true);   // Startwert Gewicht holen
  //----------------------------------------------------------------

  Serial.flush();
  Serial.end();
  Serial_rxtx_off();
}

//#########################################################
void loop() {
  byte s_alarm = 0;
  debugbegin(Serial_Baudrate);
  debugprintlnF("Loop");
  debugflush();

  Spannungen_messen();
  if (Batteriespannung > VMinimum) {
    s_alarm = measure_and_send();
    if (User_Int() == 1) {
      measure_and_send();
    }
    if (s_alarm) {  // Gewicht Alarm, senden in 10 min Intervall
      nr_to_measure = 1;
      WeckIntervallMinuten = 10;
    }
  }
  Alarm_konfigurieren();

  debugflush();
  debugend();
  Serial_rxtx_off();

  System_Off();
  SleepNow();
  System_On();
}
//#########################################################

uint8_t measure_and_send() {
  uint8_t val = 0;
  Sensor_Temp_Zelle(false);
  Sensor_DHT();
  Sensor_DS18B20();
  Sensor_Si7021();
  Sensor_SHT31();
  Sensor_BME280();
  Sensor_Licht();
  Sensor_Temp_Zelle(true);
  val = Sensor_Gewicht(false);
  if (val) nr_to_measure = 1;
  Daten_Senden();
  return (val);
}
