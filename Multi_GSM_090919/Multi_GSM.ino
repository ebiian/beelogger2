//----------------------------------------------------------------
// Funktion SIM800L - Mobilfunk  TCP / CIP-Send
//----------------------------------------------------------------

//----------------------------------------------------------------
// Konfiguration SIM800L - Mobilfunk
//----------------------------------------------------------------
#include <SIM800_beelogger_21.h>

byte send_ID_once = 1;
//----------------------------------------------------------------

//----------------------------------------------------------------
// Parameter Datenversand via Http // diese nicht modifizieren
//----------------------------------------------------------------
const char Str_Http[]   PROGMEM = " HTTP/1.1\r\n"; // mit Leerzeichen vorne
const char Str_Con[]    PROGMEM = "Connection: close\r\n\r\n";

const char Str_Pw[]     PROGMEM = "PW=";
const char Str_Id[]     PROGMEM = {"&ID="};
const char Str_Ds[]     PROGMEM = "&W=";
const char Str_T_In1[]  PROGMEM = "&T1=";
const char Str_T_In2[]  PROGMEM = "&T2=";
const char Str_T_In3[]  PROGMEM = "&T3=";
const char Str_T_In4[]  PROGMEM = "&T4=";
const char Str_T_Out[]  PROGMEM = "&TO=";
const char Str_F_In1[]  PROGMEM = "&F1=";
const char Str_F_In2[]  PROGMEM = "&F2=";
const char Str_F_In3[]  PROGMEM = "&F3=";
const char Str_F_In4[]  PROGMEM = "&F4=";
const char Str_F_Out[]  PROGMEM = "&FO=";
const char Str_Li[]     PROGMEM = "&L=";
const char Str_Serv[]   PROGMEM = "&S=";
const char Str_ServX[]  PROGMEM = "&SX=";
const char Str_Gew1[]   PROGMEM = "&G1=";
const char Str_Gew2[]   PROGMEM = "&G2=";
const char Str_Gew3[]   PROGMEM = "&G3=";
const char Str_Gew4[]   PROGMEM = "&G4=";
const char Str_Batt[]   PROGMEM = "&VB=";
const char Str_Sol[]    PROGMEM = "&VS=";
const char Str_Chk[]    PROGMEM = "&C=";
//----------------------------------------------------------------


void stop_DFUe_device() {
  digitalWrite(GSM_Power_Pin, LOW);    // SIM800 "off"
  pinMode(GSM_Power_Pin, OUTPUT);
}

void dfue_rxtx_off() {
  digitalWrite (GSM_RX, LOW);
  digitalWrite (GSM_TX, LOW);
}


//----------------------------------------------------------------
// Funktion Senden_GSM
// hier mit CIP-Funktionen für ein komplettes IP-Paket, 1.4kB
// jedes IP-Paket mit neuer Verbindung, weil Server kein "Alive"
// von mehr als 2 Sekunden erlaubt.
// Im Fehlerfall disconnect und neuer Versuch
//----------------------------------------------------------------
void Daten_Senden() {
  char parameter[60], data[16];
  float ServiceX = No_Value;
  CGPRS_SIM800 gprs;

  digitalWrite(GSM_Power_Pin, HIGH);

  debugprintlnF("GSM On!");
  debugflush();

  delay(4000);

  if (gprs.init(6000)) {  // Init SIM800 Modul

    debugprintlnF("Init!");
    debugflush();

    strcpy_P(parameter, APN);
    if (gprs.start(parameter) == 0) {  // Start GPRS Connection

      debugprintlnF("Start!");
      debugprintln(parameter);
      debugflush();

      if (report_info == 1) {
        int Signal = gprs.getSignalQuality();
        Service = (float)Signal;

        debugprintF("Pegel: ");
        debugprint(Signal);
        debugprintlnF(" dBm");
        debugflush();

      }
      else if (report_info == 2) {
        int simVoltage = gprs.getSIM800_Voltage();
        if (simVoltage > 0) {
          Service = ((float)simVoltage) / 1000.0;
        }
      }
      else if (report_info == 3) {
        GSM_LOCATION Location;
        if (gprs.getLocation(&Location)) {
          Service = Location.lat;
          ServiceX = Location.lon;

          debugprintF("Location: ");
          debugprint(Location.lat);
          debugprintF(",");
          debugprint(Location.lon);
          debugprintln();

        }
      }

      strcpy_P(parameter, serverName);
      debugprintln(parameter);
      strcpy_P(parameter, beelogger_pfad);
      debugprintln(parameter);
      debugflush();

      //#################
      byte data_send = 0;    // send status
      byte retry = 0;        // retry counter
      do {  // retry loop
        data_send = 0;
        // always new connect; session alive not supported by server

        debugprintF(" Connect ... ");
        debugflush();

        strcpy_P(parameter, serverName);
        if (gprs.Connect(parameter)) {  // Start TCP Connection

          debugprintlnF("TCP!");
          debugflush();

          float AussenTemp = SensorTemp[Aussenwerte];
          if ( Anzahl_Sensoren_Gewicht == 1) { // Vorbelegen für Temp-Korrektur
            if (AussenTemp == No_Val) AussenTemp = DS_Temp;  // Temp aus RTC
          }
          float Checksum =  SensorTemp[Beute1]  +  AussenTemp;
          Checksum +=  SensorFeuchte[Beute1]  +  SensorFeuchte[Aussenwerte];
          Checksum +=  Licht +  Gewicht[0] + Solarspannung + Batteriespannung + Service;

          if (gprs.prep_send()) { // senden wird mit \0 gestartet

            strcpy_P(parameter, beelogger_pfad); // der GET in einen Stück bis zum '?', sonst geht es nicht !!!
            gprs.send(parameter);
            // erstelle Daten
            strcpy_P(parameter, Str_Pw);
            gprs.send(parameter);
            strcpy_P(parameter, Passwort);
            gprs.send(parameter);
            if (send_ID_once) {  // System ID only at Startup
              strcpy_P(parameter, Str_Id);
              gprs.send(parameter);
              gprs.send(ID);
            }

            if ( Anzahl_Sensoren_Gewicht > 1) {
              Checksum +=  DS_Temp;
              strcpy_P(parameter, Str_Ds);
              gprs.send(parameter);
              dtostrf(DS_Temp, 3, 1, data);
              gprs.send(data);
            }

            //Temp_In
            strcpy_P(parameter, Str_T_In1);
            gprs.send(parameter);
            dtostrf(SensorTemp[Beute1], 3, 1, data);
            gprs.send(data);

            if ( Anzahl_Sensoren_Gewicht > 1) {
              Checksum +=  SensorTemp[Beute2];
              strcpy_P(parameter, Str_T_In2);
              gprs.send(parameter);
              dtostrf(SensorTemp[Beute2], 3, 1, data);
              gprs.send(data);
            }
            if ( Anzahl_Sensoren_Gewicht > 2) {
              Checksum +=  SensorTemp[Beute3];
              strcpy_P(parameter, Str_T_In3);
              gprs.send(parameter);
              dtostrf(SensorTemp[Beute3], 3, 1, data);
              gprs.send(data);
            }
            if ( Anzahl_Sensoren_Gewicht == 4) {
              Checksum +=  SensorTemp[Beute4];
              strcpy_P(parameter, Str_T_In4);
              gprs.send(parameter);
              dtostrf(SensorTemp[Beute4], 3, 1, data);
              gprs.send(data);
            }

            //TempOut
            strcpy_P(parameter, Str_T_Out);
            gprs.send(parameter);
            dtostrf(AussenTemp, 3, 1, data);
            gprs.send(data);

            //FeuchteIn
            strcpy_P(parameter, Str_F_In1);
            gprs.send(parameter);
            dtostrf(SensorFeuchte[Beute1], 3, 1, data);
            gprs.send(data);

            if ( Anzahl_Sensoren_Gewicht > 1) {
              Checksum +=  SensorFeuchte[Beute2];
              strcpy_P(parameter, Str_F_In2);
              gprs.send(parameter);
              dtostrf(SensorFeuchte[Beute2], 3, 1, data);
              gprs.send(data);
            }
            if ( Anzahl_Sensoren_Gewicht > 2) {
              Checksum +=  SensorFeuchte[Beute3];
              strcpy_P(parameter, Str_F_In3);
              gprs.send(parameter);
              dtostrf(SensorFeuchte[Beute3], 3, 1, data);
              gprs.send(data);
            }
            if ( Anzahl_Sensoren_Gewicht == 4) {
              Checksum +=  SensorFeuchte[Beute4];
              strcpy_P(parameter, Str_F_In4);
              gprs.send(parameter);
              dtostrf(SensorFeuchte[Beute4], 3, 1, data);
              gprs.send(data);
            }
            //FeuchteOut
            strcpy_P(parameter, Str_F_Out);
            gprs.send(parameter);
            dtostrf(SensorFeuchte[Aussenwerte], 3, 1, data);
            gprs.send(data);

            //Licht
            strcpy_P(parameter, Str_Li);
            gprs.send(parameter);
            dtostrf(Licht, 3, 1, data);
            gprs.send(data);

            //Service
            strcpy_P(parameter, Str_Serv);
            gprs.send(parameter);
            dtostrf(Service, 4, 2, data);
            gprs.send(data);

            if ( ServiceX != No_Value) {
              Checksum +=  ServiceX;
              strcpy_P(parameter, Str_ServX);
              gprs.send(parameter);
              dtostrf(ServiceX, 4, 2, data);
              gprs.send(data);
            }
            //Gewicht
            strcpy_P(parameter, Str_Gew1);
            gprs.send(parameter);
            dtostrf(Gewicht[0], 4, 2, data);
            gprs.send(data);
            if ( Anzahl_Sensoren_Gewicht > 1) {
              Checksum +=  Gewicht[1];
              strcpy_P(parameter, Str_Gew2);
              gprs.send(parameter);
              dtostrf(Gewicht[1], 4, 2, data);
              gprs.send(data);
            }
            if ( Anzahl_Sensoren_Gewicht > 2) {
              Checksum +=  Gewicht[2];
              strcpy_P(parameter, Str_Gew3);
              gprs.send(parameter);
              dtostrf(Gewicht[2], 4, 2, data);
              gprs.send(data);
            }
            if ( Anzahl_Sensoren_Gewicht == 4) {
              Checksum +=  Gewicht[3];
              strcpy_P(parameter, Str_Gew4);
              gprs.send(parameter);
              dtostrf(Gewicht[3], 4, 2, data);
              gprs.send(data);
            }

            //Batteriespannung
            strcpy_P(parameter, Str_Batt);
            gprs.send(parameter);
            dtostrf(Batteriespannung, 4, 2, data);
            gprs.send(data);

            //Solarspannung
            strcpy_P(parameter, Str_Sol);
            gprs.send(parameter);
            dtostrf(Solarspannung, 4, 2, data);
            gprs.send(data);

            long Check = round(Checksum);

            strcpy_P(parameter, Str_Chk);
            gprs.send(parameter);
            ltoa(Check, data, 10);
            gprs.send(data);
            // ende Daten

            strcpy_P(parameter, Str_Http);
            gprs.send(parameter);
            gprs.send("Host: ");
            strcpy_P(parameter, serverName);
            gprs.send(parameter);
            gprs.send("\r\n");

            strcpy_P(parameter, Str_Con);
            gprs.send(parameter);
            gprs.send(0x00); // Startkommando senden

            if (gprs.sendCommand(0, 5000, "200 OK")) { // Connect hat geklappt, HTTP Response Kontrolle

              debugprintF(" HTTP");
              debugflush();

              if (gprs.sendCommand(0, 5000, "ok *")) {  // response from server php
                data_send = 1; // success
                char *p_buf = strstr(gprs.buffer, "ok *");  // search string start
                byte cnt = 0;
                do {
                  p_buf --;    // search start of line
                  cnt++;
                }
                while ( (!(*p_buf == '\n')) && (cnt < 8));

                debugprintF(" Quittung OK: ");
                debugprintln(p_buf);
                debugflush();

                // Format Quittung aus beelogger-log.php
                // echo "5ok *"  = 5 Minuten
                int n = atoi(p_buf);  // Konvertiere
                if (n >= 5) {
                  WeckIntervallMinuten = n;
                }
                else WeckIntervallMinuten = WeckIntervall_default;
                send_ID_once = 0;
                report_info = 0;
                if (strchr(p_buf, 'P') ) {
                  report_info = 1;
                }
                else if (strchr(p_buf, 'V') ) {
                  report_info = 2;
                }
                else if (strchr(p_buf, 'L') ) {
                  report_info = 3;
                }
              }
              // received ok *
            }
            // http ok
          }
          // prep_send
        }
        // Connect
        gprs.disConnect();
        delay(200);
        //######################
        if (data_send == 0) {  // failed, stop and retry
          gprs.stop();         // stop CIP session

          debugprintlnF("Retry");
          debugflush();

          delay(2000);
          retry ++;
        }
      }
      while ((retry < 2) && ( data_send == 0)); // retry TCP Connect
    }
    // start
  }
  // init

  debugprintF("\nSend: ");
  debugprintF("   Weckintervall: ");
  debugprintln(WeckIntervallMinuten);
  debugflush();

  gprs.stop();        // stop CIP session
  gprs.shutdown();    // shutdown gprs
  digitalWrite(GSM_Power_Pin, LOW);
}
//----------------------------------------------------------------
