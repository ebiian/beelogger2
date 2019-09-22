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
// 08.05.2019

const char ID[] = "Waage_Kalibrierung 080519a";


//----------------------------------------------------------------
// Allgemeine Konfiguration
//----------------------------------------------------------------

// hier kann das Kalibriergewicht in _Gramm_ eingetragen werden
long KalibrierGewicht = 19000;  // 10000 = 10kG Referenzgewicht


// hier können bekannte Kalibrierwerte für Test-/Vergleichsmessung eingetragen werden
long Taragewicht[6] = {1, 1, 1, 1};
float Skalierung[6] = {1.0, 1.0, 1.0, 1.0};


//Sensorkonfiguration-----------------------------------------------------------------

#define Anzahl_Sensoren_Gewicht 1  // Mögliche Werte: '0','1','2','3','4'


// Anschluss / Konfiguration Wägezellen-------------------------------------------------------
// mit Anzahl_Sensoren_Gewicht 1
//   HX711(1) Kanal A = Wägeelement(e) Waage1
// mit Anzahl_Sensoren_Gewicht 2
//   HX711(1) Kanal B = Wägeelement(e) Waage2
// mit Anzahl_Sensoren_Gewicht 3
//   HX711(2) Kanal A = Wägeelement(e) Waage3
// mit Anzahl_Sensoren_Gewicht 4
//   HX711(2) Kanal B = Wägeelement(e) Waage4


//----------------------------------------------------------------
// Konfiguration Gewicht
//----------------------------------------------------------------
#include <HX711.h>
HX711 scale;
// hier die Adressen für die/den HX711 eintragen
byte HX711_SCK[6] = {A0, A0, 6, 6}; // HX711 Nr.1: A,B, Nr.2: A,B S-Clock
byte HX711_DT[6] =  {A1, A1, 7, 7}; // HX711 Nr.1: A,B, Nr.2: A,B Data

//----------------------------------------------------------------


//----------------------------------------------------------------
// Für beelogger-Solar: Aktivierung der Stromversorgung für Sensoren, Module und DS3231
//----------------------------------------------------------------
#define Power_Pin 4

//----------------------------------------------------------------


//----------------------------------------------------------------
// Variablen
//----------------------------------------------------------------
const float No_Val =  -1.0;  // Vorbelegung, Wert nicht gemessen
float Gewicht[6] = {No_Val, No_Val, No_Val, No_Val};
float LetztesGewicht[6] = {0, 0, 0, 0};
float DS_Temp = No_Val;
//----------------------------------------------------------------
#include <LowPower.h>
void setup() {
  char c;
  char buf[16];
  float Kal_Gew = ((float)KalibrierGewicht)/1000.0;

  Serial.begin(9600);

  Serial.println("Waage Kalibrierung");
  Serial.println();
  digitalWrite(Power_Pin, HIGH);
  pinMode(Power_Pin, OUTPUT);
  Serial.println("Zur Kalibrierung der Stockwaagen bitte den Anweisungen folgen!");
  Serial.println("Fehlerhafte und nicht angeschlossene Waagen werden auch angezeigt!");
  Serial.println("Eine Waage, die nicht kalibriert werden sollen, kann ausgelassen werden.");
  Serial.println();
  Serial.println("  Alle Waagen ohne Gewicht!");
  Serial.println();
  delay(5000);

  for (int i = 0; i < Anzahl_Sensoren_Gewicht; i++) {
    c = '#';
    if (i == 1) {
      scale.set_gain(32);  // Nr.1 Kanal B
    }
    else if (i == 3) {     // Nr.2 Kanal B
      scale.set_gain(32);
    }
    else {
      scale.begin(HX711_DT[i], HX711_SCK[i]); // Nr. 1, 2 KanalA
      scale.set_gain(128);
    }

    Serial.print("Waage Nummer: ");
    Serial.println(i + 1);
    delay(500);
    Serial.println( " Kalibrierung der Null-Lage ohne Gewicht mit '1' und 'Enter' starten!");
    Serial.println( " Eingabe von 'x': Waage wird nicht gemessen.");
    while ((c != '1') && (c != 'x')) {
      c = Serial.read();
    };
    if (c == 'x') {
      Serial.println(" ");
      if (Taragewicht[i] == 1 ) {
        LetztesGewicht[i] = -1000.0;
        Serial.println(" keine Messung ");
        Serial.println();
      }
      else {
        Serial.println(" verwende vorgegebene Daten für das Testwiegen");
      }
      Serial.println();
      continue;
    }
    c = '#';
    Serial.println();
    Serial.print("Null-Lage ... ");
    scale.set_scale(1.0);
    scale.read_average(10);

    Serial.print("  ...  ");
    Serial.flush();
    scale.read_average(20);

    Serial.println("  ...");
    Serial.flush();
    Taragewicht[i] = scale.read_average(32);
    Serial.println();

    Serial.print("Waage Nummer: ");
    Serial.println(i + 1);
    Serial.print("mit genau  ");
    dtostrf(Kal_Gew, 4, 3, buf);
    Serial.print(buf);
    Serial.println("  Kilogramm beschweren - Kalibrierung mit '2' und 'Enter' starten!");
    Serial.flush();

    while (c != '2') {
      c = Serial.read();
    };
    Serial.println();
    Serial.print("Kalibriere Waage: ");
    Serial.print(i + 1);
    Serial.flush();
    scale.set_offset(Taragewicht[i]);

    Serial.print("  ...  ");
    Serial.flush();
    scale.get_units(20);

    Serial.println("  ...");
    Serial.flush();
    Skalierung[i] = (scale.get_units(32)) / Kal_Gew;
    //Skalierung[i] =  Skalierung[i] * 1000.0; // auf Kilogramm skalieren
    scale.set_scale(Skalierung[i]);

    Serial.print("Pruefe Gewicht: ");
    Serial.flush();
    dtostrf(scale.get_units(20), 4, 3, buf);
    Serial.println(buf);

    Serial.print("Taragewicht ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(Taragewicht[i]);

    Serial.print("Skalierung  ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(Skalierung[i]);
    Serial.println();
    Serial.println();
    Serial.flush();
  }
  Serial.println();
  Serial.println("Kalibriervorgang abgeschlossen. ");
  Serial.println();
  Serial.flush();
  Serial.println("Starte Testwiegen. ");
  Serial.println("x = Kalibrierung wiederholen. ");
  Serial.println();
  Serial.flush();
  delay(200);
}

//######################################################
void loop() {
  char buf[16];
  Sensor_Gewicht(1);

  for (int i = 0; i < Anzahl_Sensoren_Gewicht; i++) {
    if (LetztesGewicht[i] == -1000.0) continue;  // keine Messung
    Serial.print("Waage  ");
    Serial.print(i + 1);
    Serial.print("  Gewicht: ");
    dtostrf(Gewicht[i], 4, 3, buf);
    Serial.print(buf);
    Serial.print(" kg");
    delay(500);
    Serial.print("   Skalierung: ");
    Serial.print(Skalierung[i]);
    Serial.print("   Taragewicht: ");
    Serial.println(Taragewicht[i]);
    Serial.flush();
    delay(1000);
  }
  char c = Serial.read();
  if (c == 'x') {
    asm volatile ("  jmp 0");
  }

  delay(5000);
}
//######################################################


//----------------------------------------------------------------
// Funktion Gewicht
//----------------------------------------------------------------
void Sensor_Gewicht(boolean quick) {
  if ((Anzahl_Sensoren_Gewicht > 0) && (Anzahl_Sensoren_Gewicht < 5)) {
    const float Diff_Gewicht = 0.5;
    //HX711 scale;
    for (int i = 0; i < Anzahl_Sensoren_Gewicht; i++) {
      if (LetztesGewicht[i] == -1000.0) continue;   // keine Messung
      Serial.print("Waage ");
      Serial.print(i + 1);
      Serial.print(" ");
      Serial.flush();
      if ((i == 1) || (i == 3)) {
        scale.set_gain(32);  // Nr.1 Kanal B
      }
      else {
        scale.begin(HX711_DT[i], HX711_SCK[i],128); // Nr. 1, 2 Kanal A
      }
      scale.set_offset(Taragewicht[i]);
      scale.set_scale(Skalierung[i]);
      LowPower.powerStandby(SLEEP_500MS, ADC_OFF, BOD_OFF);

      for (byte j = 0 ; j < 2; j++) { // Anzahl der Widerholungen, wenn Abweichung zum letzten Gewicht zu hoch
        Gewicht[i] = scale.get_units(10);
        if (quick) break;
        if (fabs(Gewicht[i] - LetztesGewicht[i]) < Diff_Gewicht) break; // Abweichung für Fehlererkennung
        LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF); // Wartezeit zwischen Wiederholungen
        Serial.print("..");
      }
      LetztesGewicht[i] = Gewicht[i];
      //  Temperaturkompensation
      if ((DS_Temp != No_Val)) {
        //ohne Kompensation Gewicht[i] = Gewicht[i] - ((DS_Temp - Kalibriertemperatur[i]) * KorrekturwertKiloGrammproGrad[i]);
      }

      if ((i == 1) || (i == 3)) {  // vier Kanal
        scale.power_down();
      }
      if ((i == 0) && (Anzahl_Sensoren_Gewicht == 1)) { // HX711 Nr. 1 abschalten
        scale.power_down();
      }
      if ((i == 2) && (Anzahl_Sensoren_Gewicht == 3)) { // HX711 Nr. 2 abschalten
        scale.power_down();
      }
      Serial.println();
      Serial.flush();
    }
  }
}
//----------------------------------------------------------------
