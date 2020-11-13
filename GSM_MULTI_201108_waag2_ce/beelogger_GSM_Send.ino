//----------------------------------------------------------------
// Funktion SIM800L - Mobilfunk  TCP / CIP-Send  EE-Prom
//----------------------------------------------------------------
// 25.04.2020 Multi Funktion als Standard
// 01.05.2020 check first char of timestammp from server
// 13.08.2020 Debug Ausgabe optimiert
//----------------------------------------------------------------
// Konfiguration SIM800L - Mobilfunk
//----------------------------------------------------------------
#include <SIM800_beelogger_22.h>

#define SERVER_TM_OUT 10000 // 10000 = 10 sec
//----------------------------------------------------------------


//----------------------------------------------------------------
// Parameter Datenversand via Http // diese nicht modifizieren
//----------------------------------------------------------------
const char Str_Http[]    PROGMEM = " HTTP/1.1\r\n";
const char Str_Con_cls[] PROGMEM = "Connection: close\r\n\r\n";

const char Str_Pw[]      PROGMEM = {"PW="};
const char Str_Opt[]     PROGMEM = {"&Z=2&A=1"};
const char Str_Id[]      PROGMEM = {"&ID="};
const char Str_Serv[]    PROGMEM = {"&S="};
const char Str_ServX[]   PROGMEM = {"&SX="};
const char Str_MData[]   PROGMEM = {"&Mx_Data="};  // Parameter Mutli Data  Senden
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
// Funktion Send_Data
// hier mit CIP-Funktionen für ein komplettes IP-Paket, 1.4kB
// jedes IP-Paket mit neuer Verbindung, weil Server kein "Alive"
// von mehr als 2 Sekunden erlaubt.
// Im Fehlerfall disconnect und neuer Versuch
//----------------------------------------------------------------
byte Send_Data(byte count) {
  char parameter[80];
  byte s_cnt = 0;            // send counter
  byte additonal_Data = 0;
  float ServiceX = No_Value;
  CGPRS_SIM800 dfue;

  digitalWrite(GSM_Power_Pin, HIGH);

  debugprintlnF("GSM On!");
  debugflush();

  delay(4000);

  if (dfue.init(6000)) {  // Init SIM800 Modul

    debugprintlnF("Init!");
    debugflush();

    strcpy_P(parameter, APN);

#if StandortBestimmung
    if ((report_info == 3) && (Service < 90.0)) {
      debugprintln(parameter);
      debugflush();
      GSM_LOCATION Location;
      if (dfue.setup(parameter, APN_Benutzer, APN_Passwort) == 0) {
        debugprintlnF(" Lat,Lon ?");
        debugflush();
        if (dfue.getLocation(&Location)) {
          Service = Location.lat;
          ServiceX = Location.lon;
          debugprint(Location.lat);
          debugprintF(",");
          debugprint(Location.lon);
          debugprintln();
          debugflush();
          additonal_Data = 1;
        }
        debugprintln(dfue.buffer);
        debugflush();
        dfue.sendCommand("AT+SAPBR=0,1");
      }
      debugflush();
    }
#endif

    if (dfue.start(parameter) == 0) {  // Start dfue Connection

      debugprintlnF("Start!");
      debugprintln(parameter);
      debugflush();

      if (Service < 90.0) {    // user action, force time_on
        if (report_info == 1) {

          int Signal = dfue.getSignalQuality();
          Service = (float)Signal;
          additonal_Data = 1;

          debugprintF(" [dBm]:");
          debugprintln(Signal);
          debugflush();

        }
        else if (report_info == 2) {
          int simVoltage = dfue.getSIM800_Voltage();
          if (simVoltage > 0) {
            Service = ((float)simVoltage) / 1000.0;
            additonal_Data = 1;
          }
        }
      }
      else {
        additonal_Data = 1;
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
        do {  // Blockweise send loop
          data_send = 0;
          // always new connect; session alive not supported by server

          debugprintlnF("Con?");
          debugflush();

          strcpy_P(parameter, serverName);
          if (dfue.Connect(parameter)) {  // Start TCP Connection

            debugprintlnF("TCP!");
            debugflush();

            if (dfue.prep_send()) {         // Prepare send data
              strcpy_P(parameter, beelogger_pfad);    // GET ....
              dfue.send(parameter);
              strcpy_P(parameter, Str_Pw);
              dfue.send(parameter);
              strcpy_P(parameter, Passwort);
              dfue.send(parameter);
              strcpy_P(parameter, Str_Opt);         //
              dfue.send(parameter);
              // System ID
              strcpy_P(parameter, Str_Id);
              dfue.send(parameter);
              strcpy(parameter, ID);
              dfue.send(parameter);
              debugprintln(parameter);
              debugflush();

              //Daten
              strcpy_P(parameter, Str_MData);
              char nmr = Anzahl_Sensoren_Gewicht + 0x30;  //itoa
              parameter[2] = nmr;  // set number in string
              dfue.send(parameter);

              debugprint(parameter);
              debugflush();

              byte y = 0;
              do {
#if EE_PROM_USE // lese von AT24Cxx
                my_EEPROM.ReadStr((s_cnt + y) * _Daten_Satz_Len, DatenString, _Daten_Satz_Len - 2);
#endif
                DatenString[(_Daten_Satz_Len - 1)] = 0;  // String ende erzwingen
                DatenString[10] = '_';  // Blank im Datum-String ersetzen
                if (y > 0) {
                  dfue.send(",");       // einfügen Satztrenner
                }
                dfue.send(DatenString);

                debugprintln(DatenString);
                debugflush();

                y++;
              } while ( (y < _Send_Sets) && ((s_cnt + y) < count) );

              if ( (additonal_Data) && ((s_cnt + y) == count) ) { // all done, Service ?
                char service_data[16];
                int count = sprintf(DatenString, "%s", "&S=");
                dtostrf(Service, 1, 6, service_data);
                count += sprintf(DatenString + count, "%s", service_data);
                if (ServiceX != No_Value) {
                  count += sprintf(DatenString + count, "%s", "&SX=");
                  dtostrf(ServiceX, 1, 6, service_data);
                  sprintf(DatenString + count, "%s", service_data);
                }
                dfue.send(DatenString);

                debugprintln(DatenString);
                debugflush();

              }
              // ende Daten
              strcpy_P(parameter, Str_Http); // HTTP/1.1
              dfue.send(parameter);
              dfue.send("Host: ");
              strcpy_P(parameter, serverName);
              dfue.send(parameter);
              dfue.send("\r\n");
              strcpy_P(parameter, Str_Con_cls);
              dfue.send(parameter);
              dfue.send(0x00);   // Start send data

              if (dfue.sendCommand(0, SERVER_TM_OUT, "200 OK")) { // WLAN und Connect haben geklappt, HTTP Response Kontrolle

                debugprintlnF("HTTP OK");
                debugflush();

                if (dfue.sendCommand(0, SERVER_TM_OUT, "ok *")) { // hat Server Anfrage quittiert ?
                  data_send = 1; // success
                  s_cnt += _Send_Sets;

                  char *p_buf = strstr(dfue.buffer, "ok *");  // search string start
                  byte pos = 0;
                  do {
                    p_buf --;    // search start of line
                    pos++;
                  }
                  while ( (!(*p_buf == '\n')) && (pos < 20));

                  debugprintF("Quittung: ");
                  debugprintln(p_buf);
                  debugflush();

                  // Format Quittung aus beelogger-log.php
                  // echo "5ok *"  = 5 Minuten
                  int n;
                  if (char *p_bf = strchr(p_buf, 'T') ) {
                    n = atoi(p_bf + 1);  // Konvertiere str in int

                    char x = *(p_buf + 1);
                    if ((x > 0x2F) && (x < 0x3A)) { // first char a number?
                      long l_tm = atol(p_buf);
                      l_tm = l_tm - 946684800;  // EPOCH_TIME_OFFSET
                      rtc.setDateTime(l_tm);
                    }
                  }
                  else {
                    n = atoi(p_buf); // Konvertiere str in int
                  }
                  if ((n >= 5) && (n < 250)) {
                    WeckIntervallMinuten = n;
                  }

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
                  if (strchr(p_buf, 'A') ) {
                    send_cycle = 1;
                  }
                  else if (strchr(p_buf, 'B') ) {
                    send_cycle = 2;
                  }
                  else if (strchr(p_buf, 'C') ) {
                    send_cycle = 4;
                  }
                  else if (strchr(p_buf, 'D') ) {
                    send_cycle = 8;
                  }
                }
                else {
                  debugprintln(dfue.buffer);
                  debugflush();
                }
                // received ok *
              }
              else {
                debugprintln(dfue.buffer);
                debugflush();
              }
              // Http OK  0,200 ?
            }
            // prep_send
          }
          // Connect
          dfue.disConnect();
          delay(200);
        }
        while ((s_cnt < count) && (data_send == 1)); // while my_counter
        //######################
        if (data_send == 0) {  // failed, stop and retry
          dfue.stop();        // stop CIP session

          debugprintlnF("Retry");
          debugflush();

          delay(2000);
          retry ++;
        }
      }
      while ((retry < 2) && (data_send == 0)); // retry TCP Connect
    }
    // start
  }
  // init
#if EE_PROM_USE
  debugprintF("\nSend: ");
  debugprintln(s_cnt);  debugprintF(" Zyklus: ");
  debugprintln(send_cycle);
  debugflush();
#endif

  dfue.stop();        // stop CIP session
  dfue.shutdown();    // shutdown dfue
  delay(50);
  dfue_rxtx_off();
  digitalWrite(GSM_Power_Pin, LOW);
  return (s_cnt);
}
//----------------------------------------------------------------
