
//----------------------------------------------------------------
//Sensorkonfiguration
//----------------------------------------------------------------
#define Anzahl_Sensoren_DHT     0 // Mögliche Werte: '0','1','2'  --- Nr 1+2 ---- (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_Si7021  0 // Mögliche Werte: '0','1'      --- Nr 3 -----  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_SHT31   0 // Mögliche Werte: '0','1','2'  --- Nr 4+5 ---  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_BME280  0 // Mögliche Werte: '0','1','2'  --- Nr 6+7 ---  (Temperatur + Luftfeuchte)

#define Anzahl_Sensoren_DS18B20 1 // Mögliche Werte: '0','1','2'  --- Nr 8+9 ---  (Nur Temperatur)

// Sensormatrix, hier kann die Zuordnung der Sensoren geändert werden                                            
// Nr aus Liste oben auswählen, wenn kein Sensor gewünscht ist einfach "0" angeben
// wenn kein Sensor für die Aussentemperatur gesetzt ist, wird automatisch der Temperatursensor der RTC verwendet
#define             Aussenwerte                                          8
#define             Beute1                                               1
// mit zwei Waagen
#define             Beute2                                               0
// mit drei Waagen
#define             Beute3                                               0
// mit vier Waagen
#define             Beute4                                               0


#define Anzahl_Sensoren_Licht   0  // Mögliche Werte: '0','1'      

#define Anzahl_Sensoren_Gewicht 1 // Mögliche Werte: '0','1','2','3','4'

// Anschluss / Konfiguration Wägezellen-------------------------------------------------------
// mit Anzahl_Sensoren_Gewicht 1    
//   HX711(1) Kanal A = Wägeelement(e) Waage1;   Serverskript: beeloggerY
// mit Anzahl_Sensoren_Gewicht 2 zusätzlich: 
//   HX711(1) Kanal B = Wägeelement(e) Waage2;   Serverskript: DuoY
// mit Anzahl_Sensoren_Gewicht 3 zusätzlich:
//   HX711(2) Kanal A = Wägeelement(e) Waage3;   Serverskript: TripleY
// mit Anzahl_Sensoren_Gewicht 4 zusätzlich:
//   HX711(2) Kanal B = Wägeelement(e) Waage4;   Serverskript: QuadY

//--------------------------------------------------------------------------------------------
// Kalibrierwerte für die Wägezellen
const long Taragewicht[4] = { 10,  10,  10,  10 }; // Hier ist der Wert aus der Kalibrierung einzutragen
const float Skalierung[4] = { 1.0, 1.0, 1.0, 1.0 }; // Hier ist der Wert aus der Kalibrierung einzutragen

//--------------------------------------------------------------------------------------------
// Kalibrierwerte für die Spannungsmessung Akku
const long Kalib_Spannung = 4120;    // Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
const long Kalib_Bitwert  =  1631;    // Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
//----------------------------------------------------------------



//----------------------------------------------------------------
// GSM Konfiguration
//----------------------------------------------------------------
// Name des Zugangspunktes des Netzwerkproviders
const char  APN[] PROGMEM = {"mass.at"};//("internet.t-mobile");("web.vodafone.de");

// Domainname zum Webserver mit beelogger-Skript
// Länge maximal 30 Zeichen
const char serverName[] PROGMEM = "testsystem.beelogger.de";

// GET mit Verzeichnis auf dem Webserver und PHP-Skript für den jeweiligen beelogger
// Mit Anzahl_Sensoren_Gewicht
// 1 - Bsp: "GET /USERX/beeloggerY/beelogger_log.php?"
// 2 - Bsp: "GET /USERX/DuoY/beelogger_log.php?"
// 3 - Bsp: "GET /USERX/TripleY/beelogger_log.php?"
// 4 - Bsp: "GET /USERX/QuadY/beelogger_log.php?"
const char beelogger_pfad[] PROGMEM = {"GET /ebiian/beelogger2/beelogger_log.php?"};   // "USERX" und "SystemY" ersetzen

// Passwort vom Webserver-Skript
const char Passwort[] PROGMEM =  {"Log"};  // hier dein beelogger-log - Passwort
//----------------------------------------------------------------




//----------------------------------------------------------------
// Allgemeine Konfiguration
//----------------------------------------------------------------
#define WeckIntervall_default 15
#define AlternativIntervallMinuten  120  // Weckinterval, wenn VAlternativ erreicht
// Li-Ion Akku beelogger-Solar
const float VAlternativ = 3.8;       // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
const float VMinimum    = 3.75;      // Minimale Spannung ab der ab der keine Messungen und auch kein Versand von Daten erfolgt

// 6V PB-Akku beelogger-Universal
//const float VAlternativ = 5.9;       // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
//const float VMinimum    = 5.7;       // Minimale Spannung ab der ab der keine Messungen und auch kein Versand von Daten erfolgt

// 12V PB-Akku beelogger-Universal
//const float VAlternativ = 11.9;     // Minimale Spannung ab der automatisch das alternative Intervall aktiviert wird
//const float VMinimum = 11.5;        // Minimale Spannung ab der keine Messungen und auch kein Versand von Daten erfolgt
//----------------------------------------------------------------
