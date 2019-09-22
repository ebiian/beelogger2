/*
   (C) 2019 R.Schick - beelogger.de

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
// Version 2019    09.09.2019  beelogger Multi GSM, bis zu vier Waagen

const char ID[] = "MULTI_GSM_090919_waage2_15092019_ce";  //nur Grossbuchstaben,Zahlen, keine Blanks


#include "Multi_GSM_config.h"     // Konfiguration und Kalibrierung eintragen
#include "beelogger_config.h"


//----------------------------------------------------------------
// Variablen
//----------------------------------------------------------------
const float No_Val = 99.9;   // Vorbelegung, Wert nicht gemessen
const float No_Value = -1.0; // Vorbelegung, Wert nicht gemessen

//                     KeinSensor DHT1, DHT1,   Si7021, SHT31a, SHT31b, BMEa,   BMEb,  DS18B20a,DS18B20b
float SensorTemp[10] = {No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val, No_Val};

//                        KeinSensor  DHT1,    DHT2,     Si7021,   SHT31a,   SHT31b,   BMEa,     BMEb.   KeinSensor KeinSensor
float SensorFeuchte[10] = {No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value, No_Value};
float DS_Temp = No_Val;
float Licht = No_Value;
float Gewicht[4] = {No_Value, No_Value, No_Value, No_Value};
float LetztesGewicht[4] = {No_Value, No_Value, No_Value, No_Value};

float Batteriespannung = No_Val;
float Solarspannung = No_Val;

float Service = No_Value;

int WeckIntervallMinuten = WeckIntervall_default;
DateTime aktuell;

uint8_t time_on = 0;

byte report_info = 0;     // 0 = time-On, 1 = Pegel, 2 = SIM800volt, 3 = Location

volatile bool ok_sleep = false;

//----------------------------------------------------------------


void setup() {

  Serial.begin(Serial_Baudrate);
  Serial.println(ID);

  //----------------------------------------------------------------
  // System On
  //----------------------------------------------------------------
  stop_DFUe_device();

  Spannungen_messen();
  while (Batteriespannung < VMinimum) {
    LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_OFF);
    Spannungen_messen();
  }
  digitalWrite(Power_Pin, HIGH);
  pinMode(Power_Pin, OUTPUT);
  delay(5);

  setup_hx711();

  //----------------------------------------------------------------
  //  Setup RTC & Sleep-Mode
  //----------------------------------------------------------------
  rtc.begin();
  rtc.clearINTStatus();

  DateTime pc_tim = DateTime(__DATE__, __TIME__);
  long l_pczeit = pc_tim.get();
  aktuell = rtc.now();
  long l_zeit = aktuell.get();
  if (l_pczeit > l_zeit) {
    rtc.setDateTime(l_pczeit);
  }
  time_on = aktuell.second();
  
  //----------------------------------------------------------------
  //  Sleep Mode & Send data Interrupt
  //----------------------------------------------------------------
  pinMode(DS3231_Interrupt_Pin, INPUT_PULLUP);
  ok_sleep = true;
  delay(5);
  //----------------------------------------------------------------

  //----------------------------------------------------------------
  // Setup Gewicht
  //----------------------------------------------------------------
  Sensor_Gewicht(true);   // Startwert Gewicht holen
  //----------------------------------------------------------------

  Serial.flush();
  Serial.end();
  Serial_rxtx_off();
}


//##################################################################
void loop() {

  debugbegin(Serial_Baudrate);
  debugprintlnF("Loop");
  debugflush();

  Spannungen_messen();
  if (Batteriespannung > VMinimum) {
    DS_Temp = No_Val;
    Sensor_DHT();
    Sensor_SHT31();
    Sensor_DS18B20();
    Sensor_Si7021();
    Sensor_BME280();
    if (DS_Temp == No_Val) DS_Temp = rtc.getTemperature();
    Sensor_Gewicht(false);
    Sensor_Licht();
    delay(1000);
    Daten_Senden();
  }
  Alarm_konfigurieren();

  debugflush();
  debugend();
  Serial_rxtx_off();

  SleepNow();
}
//##################################################################
