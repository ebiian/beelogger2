# beelogger2
 beelogger.de
waage 2 ebi
am 06.10.2019 neu geflasht !
ori.arduino IDE(1.8.9):Board:Arduino PRO or Pro Mini; processor:ATmega 328P (3V,8MHZ);Programmer:Arduino as ISP
ACHTUNG: "Sketch->Upload using Programmer" zum programmieren drücken !!!

APN[] PROGMEM = {"mass.at"
serverName[] PROGMEM = "testsystem.beelogger.de"
beelogger_pfad[] PROGMEM = {"GET /ebiian/beelogger2/beelogger_log.php?"}

const long Taragewicht[4] = { 128385,  10,  10,  10 }; // Hier ist der Wert aus der Kalibrierung einzutragen
const float Skalierung[4] = { 20475.42, 1.0, 1.0, 1.0 }; // 
// Kalibrierwerte für die Spannungsmessung Akku
const long Kalib_Spannung = 4120;    // Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht
const long Kalib_Bitwert  =  1631;    // Hier muss der Wert aus der Kalibrierung eingetragen werden, sonst funktioniert der Programmcode nicht

am 26.10.2020 neuer akku! (von 2 auf 5 stk 18650 aufgerüstet)
problem: es wird alle 15 min. werden 2 messungen! gesendet obwohl 240 min. eingestellt ist !?!
