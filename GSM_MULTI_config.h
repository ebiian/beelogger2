//----------------------------------------------------------------
// Sensorkonfiguration
// 1. Sensor für die Abfrage durch den Sketch aktivieren
// 2. aktive Sensoren für Temperatur/Feuchte in der Sensormatrix zuordnen
// 3. Sensor für Temperatur Wägezelle festlegen
//----------------------------------------------------------------
#define Anzahl_Sensoren_DHT     0 // Mögliche Anzahl: '0','1','2'  --- Nr 1,2 ---  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_Si7021  0 // Mögliche Anzahl: '0','1'      --- Nr 3 -----  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_SHT31   0 // Mögliche Anzahl: '0','1','2'  --- Nr 4,5 ---  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_BME280  0 // Mögliche Anzahl: '0','1','2'  --- Nr 6,7 ---  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_DS18B20 1 // Mögliche Anzahl: '0','1','2'  --- Nr 8,9 ---  (Nur Temperatur)
// Mögliche Anzahl:     '3','4'  --- Nr 8,9 ---  (im Messwert Luftfeuchte)

// 2. Sensormatrix, hier kann die Zuordnung der Sensoren geändert werden
// Nr 1 - 9 aus Liste oben auswählen, wenn kein Sensor gewünscht ist einfach "0" angeben
// wenn kein Sensor für die Aussentemperatur gesetzt ist, wird automatisch der Temperatursensor der RTC verwendet
#define             Aussenwerte                                          0    // 0 oder Nr. 1 - 9
// Sensor Beute 1
#define             Beute1                                               0    // 0 oder Nr. 1 - 9
// Sensor Beute 2
#define             Beute2                                               0    // 0 oder Nr. 1 - 9
// mit drei Waagen
#define             Beute3                                               0    // 0 oder Nr. 1 - 9
// mit vier Waagen
#define             Beute4                                               0    // 0 oder Nr. 1 - 9
// mit fünf Waagen
#define             Beute5                                               0    // 0 oder Nr. 1 - 9
// mit sechs Waagen
#define             Beute6                                               0    // 0 oder Nr. 1 - 9

//----------------------------------------------------------------
// Temperatur Wägezelle (Duo, Tripple, Quad usw.
// Sensor, der die Temperatur der Wägezelle erfasst;  vorbelegt der erste DS18B20
// für Systeme mit einer Waage identisch zum Sensor Aussenwerte eintragen
#define             Temp_Zelle                                           8    // 0 oder Nr. 1 - 9



//----------------------------------------------------------------
// erweiterte Sensoren
//----------------------------------------------------------------
#define Anzahl_Sensor_Luftdruck   0 // Mögliche Anzahl: '0','1'  --- Aux 1 ---- (Luftdruck von BME280)
#define Hoehe_Standort          0.0 // Höhe des Standorts über NN in Meter für Korrektur des Messwertes Luftdruck

#define Anzahl_Sensor_Regen       0 // Mögliche Anzahl: '0','1'  --- Aux 2 ---- (Regenmesser, erfordert Umbau des beelogger und zusätzliche INO)

#define Anzahl_Sensor_xyz         0 // Mögliche Anzahl: '0','1'  --- Aux 3 ---  (beliebige Erweiterung)


#define Anzahl_Sensoren_Licht     0 // Mögliche Anzahl: '0','1'      


//----------------------------------------------------------------
// Konfiguration Waage(n)
//----------------------------------------------------------------
#define Anzahl_Sensoren_Gewicht 1 // Mögliche Anzahl: '0','1','2','3','4','5','6'

// Anschluss / Konfiguration Wägezellen-------------------------------------------------------
// mit Anzahl_Sensoren_Gewicht 1
//   HX711(1) Kanal A = Wägeelement(e) Waage1;   Serverskript: beeloggerY
// mit Anzahl_Sensoren_Gewicht 2 zusätzlich:
//   HX711(1) Kanal B = Wägeelement(e) Waage2;   Serverskript: DuoY

// mit Anzahl_Sensoren_Gewicht 3 zusätzlich:
//   HX711(2) Kanal A = Wägeelement(e) Waage3;   Serverskript: TripleY
// mit Anzahl_Sensoren_Gewicht 4 zusätzlich:
//   HX711(2) Kanal B = Wägeelement(e) Waage4;   Serverskript: QuadY

// mit Anzahl_Sensoren_Gewicht 5 zusätzlich:
//   HX711(3) Kanal A = Wägeelement(e) Waage5;   Serverskript: PentaY
// mit Anzahl_Sensoren_Gewicht 6 zusätzlich:
//   HX711(3) Kanal B = Wägeelement(e) Waage6;   Serverskript: HexaY

//--------------------------------------------------------------------------------------------
// Kalibrierwerte für die Wägezellen
//const long Taragewicht[6] = { 10,  10,  10,  10,  10 , 10 }; // Hier ist der Wert aus der Kalibrierung einzutragen
//const float Skalierung[6] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }; // Hier ist der Wert aus der Kalibrierung einzutragen
const long Taragewicht[4] = { 128385,  10,  10,  10 }; // Hier ist der Wert aus der Kalibrierung einzutragen
const float Skalierung[4] = { 20475.42, 1.0, 1.0, 1.0 }; // Hier ist der Wert aus der Kalibrierung einzutragen

//--------------------------------------------------------------------------------------------
// Kalibrierwerte für die Spannungsmessung Akku
//const long Kalib_Spannung = 1000;    // Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
//const long Kalib_Bitwert  =  100;    // Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
const long Kalib_Spannung = 4120;    //395 Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
const long Kalib_Bitwert  =  1631;    //395 Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
//----------------------------------------------------------------



//----------------------------------------------------------------
// GSM Konfiguration
//----------------------------------------------------------------

// Name des Zugangspunktes des Netzwerkproviders
//const char  APN[] PROGMEM = {"APN-Name"};//("internet.t-mobile");("web.vodafone.de");("internet.eplus.de");
const char  APN[] PROGMEM = {"mass.at"};//spusu 395 ("internet.t-mobile");("web.vodafone.de");

// Standort nur mit SIM800 Firmware Version: "1418B05SIM800L2"
#define StandortBestimmung  0       // soll der Standort des beeloggers vom Server aus abgerufen werden können? 0=nein 1= ja

// APN Benutzername/Passwort: Werte nicht erforderlich
#if StandortBestimmung
static char APN_Benutzer[] = {""};
static char APN_Passwort[] = {""};
#endif


// Domainname zum Webserver mit beelogger-Skript
// Länge maximal 30 Zeichen
const char serverName[] PROGMEM = "community.beelogger.de";

// GET mit Verzeichnis auf dem Webserver und PHP-Skript für den jeweiligen beelogger
// Mit Anzahl_Sensoren_Gewicht
// 1 - Bsp: "GET /USERX/beeloggerY/beelogger_log.php?"
// 2 - Bsp: "GET /USERX/DuoY/beelogger_log.php?"
// 3 - Bsp: "GET /USERX/TripleY/beelogger_log.php?"
// 4 - Bsp: "GET /USERX/QuadY/beelogger_log.php?"
// 5 - Bsp: "GET /USERX/PentaY/beelogger_log.php?"
// 6 - Bsp: "GET /USERX/HexaY/beelogger_log.php?"
//const char beelogger_pfad[] PROGMEM = {"GET /USERX/SystemY/beelogger_log.php?"};   // "USERX" und "SystemY" ersetzen
const char beelogger_pfad[] PROGMEM = {"GET /ebiian/beelogger2/beelogger_log.php?"};   // "USERX" und "SystemY" ersetzen

// Passwort vom Webserver-Skript
const char Passwort[] PROGMEM =  {"Log"};   // hier dein beelogger-log - Passwort
//----------------------------------------------------------------


//----------------------------------------------------------------
// Allgemeine Konfiguration
//----------------------------------------------------------------
#define WeckIntervall_default 60  //395 15
#define AlternativIntervallMinuten  240 //395 120  // Weckinterval, wenn VAlternativ erreicht
// Li-Ion Akku beelogger-Solar
const float VAlternativ = 3.8;       // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
const float VMinimum    = 3.75;      // Minimale Spannung ab der ab der keine Messungen und auch kein Versand von Daten erfolgt

// 6V PB-Akku beelogger-Universal
//const float VAlternativ = 5.9;       // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
//const float VMinimum    = 5.7;       // Minimale Spannung ab der ab der keine Messungen und auch kein Versand von Daten erfolgt

// 2x LiIon-Akku beelogger-Universal
//const float VAlternativ = 7.6;     // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
//const float VMinimum = 7.5;        // Minimale Spannung ab der keine Messungen und auch kein Versand von Daten erfolgt

// 12V PB-Akku beelogger-Universal
//const float VAlternativ = 11.9;     // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
//const float VMinimum = 11.5;        // Minimale Spannung ab der keine Messungen und auch kein Versand von Daten erfolgt
//----------------------------------------------------------------


//----------------------------------------------------------------
// Konfiguration Datenablage EEProm
// Anpassungen nur mit Vorsicht.
//----------------------------------------------------------------
#define EEProm_Size 4096       // 4kByte EE-Prom    32kbit (z.B. DS3231 Modul)
//#define EEProm_Size 8192     // 8kByte EE-Prom    64kbit
//#define EEProm_Size 32768    // 32kByte EE-Prom  256kbit


// ************ ENDE Konfiguration ********************

#if ( Aux1 == 0 ) && ( Aux2 == 0 ) && ( Aux3 == 0 )
#define Daten_Satz_part 80
#endif

#if Aux1 || Aux2 || Aux3
#define Daten_Satz_part 88
#endif

#if (Aux1 && Aux2) || (Aux3 && Aux2)
#define Daten_Satz_part 96
#endif

#if (Aux1 && Aux2 && Aux3)
#define Daten_Satz_part 96
#endif

#define _Daten_Satz_Len (Daten_Satz_part + (Anzahl_Sensoren_Gewicht*16))    // Datensatz  16/32byte increment step!!! 

#define _Num_Measured   ((EEProm_Size / _Daten_Satz_Len) - _Num_Retry)  // maximale Anzahl Messungen pro Uebertragungsserie
#define _Num_Retry       1    // Anzahl Sendewiederholungen für den gesamten Datensatz bei Verbindungsfehlern

#if Anzahl_Sensoren_Gewicht > 4
#define _Send_Sets       5    // 5 Datenpakete pro TCP Send plus http-overhead
#elif Anzahl_Sensoren_Gewicht == 4
#define _Send_Sets       6    // 6 Datenpakete pro TCP Send plus http-overhead
#else
#define _Send_Sets       7    // 7 Datenpakete pro TCP Send plus http-overhead
#endif
