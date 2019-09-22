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
   Version 22.07.2019
*/

// beelogger.de - Arduino Datenlogger für Imker
// Erläuterungen dieses Programmcodes unter http://beelogger.de


#include <Sodaq_DS3231.h>
#include <Wire.h>

int Kalib_Spannung = 4150; //Hier ist der gemessene Wert der Akkuspannung in Millivolt einzutragen
int Kalib_Bitwert = 0;

float Batteriespannung = 999.99;

byte Power_Pin = 4;

void setup() {

  Serial.begin(9600);
  Serial.println("beelogger Kalibierung ADC");
  

  pinMode(Power_Pin, OUTPUT);
  digitalWrite(Power_Pin, HIGH);
  delay(5);
  Serial.println("Starte Uhrbaustein:");
  Serial.flush();
  rtc.begin();
  rtc.setDateTime(1); // start RTC
  delay(50);
  DateTime pc_tim = DateTime(__DATE__, __TIME__);
  long l_pczeit = pc_tim.get();
  DateTime aktuell = rtc.now();
  long l_zeit = aktuell.get();
  if (l_pczeit > l_zeit)  rtc.setDateTime(l_pczeit);
  delay(50);
  digitalWrite(Power_Pin, LOW);
  Serial.println("Uhrbaustein initialisiert.");
  Serial.flush();
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);
  if (Kalib_Spannung == 0) {
    Serial.println("Kein Wert fuer 'Kalib_Spannung' eingetragen.");
    Serial.println("Die Kalibrierung kann nicht durchgefuehrt werden.");
    while (true) {};
  }

  Kalib_Bitwert = analogRead(A6);
  Kalib_Bitwert = 0;
  for (byte j = 0 ; j < 16; j++) {
    Kalib_Bitwert += analogRead(A6);
  }
  Kalib_Bitwert = Kalib_Bitwert >> 2;

  Serial.print("Hinterlegter Wert fuer 'Kalib_Spannung': ");
  Serial.println(Kalib_Spannung);
  Serial.println();
  Serial.print("Gemessener Wert fuer 'Kalib_Bitwert': ");
  Serial.println(Kalib_Bitwert);
  Serial.println();

  Batteriespannung = (map(Kalib_Bitwert, 0, Kalib_Bitwert, 0, Kalib_Spannung)) / 1000.0;

  Serial.print("Die mit dieser Kalibrierung ermittelte Akkuspannung betraegt:");
  Serial.print(Batteriespannung);
  Serial.println(" V");
  Serial.println();
}


void loop() {

}
