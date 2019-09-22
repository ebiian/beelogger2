/*
   (C) 2018 R.Schick / Thorsten Gurzan - beelogger.de

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// beelogger.de - Arduino Datenlogger für Imker
// Erläuterungen dieses Programmcodes unter http://beelogger.de


#include <Sodaq_DS3231.h>
#include <Wire.h>
#include <LowPower.h>


// Intervalle
//----------------------------------
byte WeckIntervallMinuten = 1;
byte AlternativIntervallMinuten = 6;
float VAlternativ = 3.8;             // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
float VMinimum = 3.75;               // Minimale Spannung ab der keine Messungen und auch kein Versand von Daten erfolgt
//----------------------------------

// Kalibrierung
//----------------------------------
int Kalib_Spannung = 4150; // Hier muss der Wert aus der Kalibrierung eingetragen werden
int Kalib_Bitwert  =  1651; // Hier muss der Wert aus der Kalibrierung eingetragen werden
//----------------------------------

float Batteriespannung = -1.0;
float Solarspannung = -1.0;
float RTCTemp = 99.9;

volatile bool ok_sleep = false;

#define USB_RX      0          // Pin 0 RX-USB
#define USB_TX      1          // Pin 1 TX-USB
#define DS3231_Interrupt_Pin  2

#define Power_Pin  4

#define Batterie_messen    A6
#define Solarzelle_messen  A7


//--------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("beelogger Solar Test 15.07.2019 ");
  Serial.println("beelogger Solar Test Uhr und Kalibrierwerte: ");

  pinMode(Power_Pin, OUTPUT);
  digitalWrite(Power_Pin, HIGH);

  if ( Kalib_Bitwert == 100) {
    Serial.println();
    Serial.println();
    Serial.println("Bitte Kalibrierwerte eintragen! ");
    Serial.println("Bitte Kalibrierwerte eintragen! ");
    Serial.println("Bitte Kalibrierwerte eintragen! ");
    Serial.println();
    Serial.println();
    WeckIntervallMinuten = 10;
  }
  Spannungen_messen();
  delay(5);

  rtc.begin();

  DateTime pc_tim = DateTime(__DATE__, __TIME__);
  long l_pczeit = pc_tim.get();
  DateTime aktuell = rtc.now();
  long l_zeit = aktuell.get();
  if (l_pczeit > l_zeit)  rtc.setDateTime(pc_tim.get());

  rtc.clearINTStatus();

  pinMode(DS3231_Interrupt_Pin, INPUT_PULLUP);
  ok_sleep = true;

  delay(5);

  display_time();
  display_interval();
  Alarm_konfigurieren();
  SleepNow(false);
}
//--------------------------------------------------


//##################################################
void loop() {
  Spannungen_messen();
  display_time();
  display_temp();
  display_interval();
  Alarm_konfigurieren();
  SleepNow(true);
}
//##################################################


//--------------------------------------------------
void display_time() {
  DateTime aktuell = rtc.now();
  Serial.print("Datum und Uhrzeit: ");
  Serial.print(aktuell.date(), DEC);
  Serial.print('.');
  Serial.print(aktuell.month(), DEC);
  Serial.print('.');
  Serial.print(aktuell.year(), DEC);
  Serial.print(' ');
  Serial.print(aktuell.hour(), DEC);
  Serial.print(':');
  Serial.print(aktuell.minute(), DEC);
  Serial.print(':');
  Serial.print(aktuell.second(), DEC);
  Serial.println();
  Serial.flush();
}
//--------------------------------------------------


//--------------------------------------------------
void display_temp() {
  RTCTemp = rtc.getTemperature();
  Serial.print("Temperatur ueber Sensor in RTC: ");
  Serial.println(RTCTemp);
  Serial.println();
  Serial.flush();
}
//--------------------------------------------------


//--------------------------------------------------
void display_interval() {
  Serial.println();
  Serial.print("Eingestelltes Weckintervall: ");
  Serial.print(WeckIntervallMinuten);
  Serial.println(" Minute(n)");
  Serial.flush();
}
//--------------------------------------------------


//--------------------------------------------------
void Spannungen_messen() {
  Batteriespannung = Messe_Spannung(Batterie_messen);
  Serial.print(F(" Batterie [V]: "));
  Serial.println(Batteriespannung);
  Serial.flush();
  if (Batteriespannung > VMinimum) {
    Solarspannung = Messe_Spannung(Solarzelle_messen);
    Serial.print(F(" Solarspannung [V]: "));
    Serial.println(Solarspannung);
    Serial.flush();
  }
}

float Messe_Spannung (byte Pin) {
  int Messung_Spannung;
  float Spannung;
  Messung_Spannung = analogRead(Pin);
  Messung_Spannung = 0;
  for (byte j = 0 ; j < 16; j++) {
    Messung_Spannung += analogRead(Pin);
  }
  Messung_Spannung = Messung_Spannung >> 2;
  Spannung = (float)map(Messung_Spannung, 0, Kalib_Bitwert, 0, Kalib_Spannung) / 1000.0;
  return (Spannung);
}
//--------------------------------------------------


//--------------------------------------------------
void Alarm_konfigurieren() {
  byte Minute, IntervallMinuten;

  if (Batteriespannung > VAlternativ) {
    IntervallMinuten = WeckIntervallMinuten;
  } else {
    Serial.println("Batteriespannung unter Grenzwert, alternatives Intervall aktiviert");
    Serial.println();
    IntervallMinuten = AlternativIntervallMinuten;
  }
  DateTime aktuell = rtc.now();
  long timestamp = aktuell.get();
  aktuell = timestamp + IntervallMinuten * 60;
  rtc.enableInterrupts(aktuell.hour(), aktuell.minute(), 0);
}
//--------------------------------------------------


//--------------------------------------------------
void WakeUp() {
  detachInterrupt(0);
  ok_sleep = false;
}
//--------------------------------------------------


//--------------------------------------------------
void SleepNow(byte power_off) {
  delay(100);
  if (power_off) {
    Serial.println("Sleep-Modus mit Power-Off ist aktiviert, bitte warten ... ");
    Serial.flush();
    digitalWrite(Power_Pin, LOW);
    delay(5);
  }
  else {
    Serial.println("Sleep-Modus mit Power-ON gestartet, bitte warten ... ");
    Serial.flush();
  }
  Serial.end();        // Serial aus
  delay(50);
  digitalWrite(USB_RX, LOW);   // Port aus
  pinMode(USB_TX, INPUT);
  digitalWrite(USB_TX, LOW);   // Port aus

  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);

  delay(1);
  attachInterrupt(0, WakeUp, LOW);
  if (ok_sleep) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
  else {
    Serial.begin(9600);
    Serial.println("Sleep-Modus konnte nicht aktiviert werden");
  }

  digitalWrite(Power_Pin, HIGH);
  delay (5);
  rtc.begin();
  rtc.clearINTStatus();
  while (digitalRead(DS3231_Interrupt_Pin) == false) {};
  ok_sleep = true;

  Serial.begin(9600);
  Serial.println("Sleep-Modus wurde beendet");
  Serial.println();
  delay(5);
}
//--------------------------------------------------
