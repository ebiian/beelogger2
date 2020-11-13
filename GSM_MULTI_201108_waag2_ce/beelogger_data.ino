//09.03.2020
//02.04.2020  HEXA, 6 Waagen
//25.04.2020  user_interrupt handling
//07.08.2020  EE-Prom Checksumme
//18.08.2020  Sendezeit bei Netzausfall angepasst
// 25.10.2020  Temperatur Waegezelle
//----------------------------------------------------------------
// Funktion Store_or_Send
//   Daten in EE-Prom ablegen
//   wenn Anzahl Messungen oder SendeZyklus erreicht: Senden
//----------------------------------------------------------------

void Daten_Senden() {

  DatenString_erstellen();
  debugprintln(DatenString);

#if EE_PROM_USE
  // Speichere in AT24Cxx
  uint8_t x, sum = 0;  // Checksumme
  for (x = 0; x < strlen(DatenString); x++) {
    sum += DatenString[x];
  }
  DatenString[_Daten_Satz_Len - 1] = sum; // EE_Prom Checksum
  my_EEPROM.WriteMem(my_counter * _Daten_Satz_Len, DatenString, _Daten_Satz_Len);
  my_counter++;

  debugprintF("Nr.: ");
  debugprint(my_counter);
  debugprintF("  len=");
  debugprintln(strlen(DatenString));
  debugflush();


  uint8_t xhour = aktuell.hour();
  uint8_t time_to_send = 0;

  if ((next_send_hour > 6) || (xhour <= 6)) { // schedule past 6 or time before 6
    if (xhour >= next_send_hour) {            //
      time_to_send = 1;
    }
  }
  if ((next_send_hour == 6) && (xhour > 5) && (xhour < 12)) { // schedule at 6o'clock
    time_to_send = 1;
  }

  if ((my_counter >= nr_to_measure) ||    //  Anzahl Messungen  oder
      ( time_to_send )) {                 //  Sendezeit erreicht

    if (Send_Data(my_counter)) {

      my_counter = 0;
      calc_send_hour(xhour, 0);

    }
    else {  // Sendefehler
      debugprintF(" Senden Fehler");
      calc_send_hour(xhour, 1);
      if (my_counter >= (_Num_Measured + _Num_Retry)) { // extra Versuche erreicht
        /*
          debugprintln("\nCheck:   ");
          debugflush();
          for (uint8_t a = 0; a < my_counter; a++) {
          my_EEPROM.ReadMem(a * _Daten_Satz_Len, DatenString, _Daten_Satz_Len);
          debugprintln(DatenString);
          debugflush();
          }
        */
        uint8_t x = my_counter / 2;
        for (uint8_t i = 0; i < x; i++) {
          my_EEPROM.ReadMem(i * 2 * _Daten_Satz_Len, DatenString, _Daten_Satz_Len);
          my_EEPROM.WriteMem(i * _Daten_Satz_Len, DatenString, _Daten_Satz_Len);
        }
        /*
          debugprintln("compressed:   ");
          for (uint8_t b = 0; b < x; b++) {
            my_EEPROM.ReadMem(b * _Daten_Satz_Len, DatenString, _Daten_Satz_Len);
            debugprintln(DatenString);
            debugflush();
          }
          debugprintln("   ");
        */
        my_counter = x; // Messdaten teilweise verwerfen
      }
    }

    nr_to_measure = _Num_Measured;  // set to normal number to measure
  } // if counter

#else

  Send_Data(1);  // normaler beelogger

#endif


  // Check Intervall
  if (WeckIntervallMinuten < 5) WeckIntervallMinuten = WeckIntervall_default;

  debugprintF(" Intervall: ");
  debugprintln(WeckIntervallMinuten);
  debugflush();

}
//----------------------------------------------------------------


//----------------------------------------------------------------
// calculate next time to send data
//----------------------------------------------------------------
void calc_send_hour(uint8_t xhour, uint8_t failed ) {
  uint8_t s_cycle;

  if (failed) {
    s_cycle = 1;                      // Sendefehler neuer Versuch
  }
  else {
    s_cycle = send_cycle;             //  current information from server
    if (WeckIntervallMinuten > 30) s_cycle = send_cycle * 2;
  }

  if (s_cycle > 24 ) s_cycle = 24;  // at least once a day

  next_send_hour = (xhour + s_cycle) % 24;  // next time to send
  if (next_send_hour <= xhour) {            // next day
    if (next_send_hour > 6) {           // beyond 6 o'clock
      next_send_hour = 6;               // force send at 6 o'clock
    }
  }

  if ((xhour < 6) && ((xhour + s_cycle) > 6)) {  // before 6 o'clock
    next_send_hour = 6;                   // force send at 6 o'clock
  }

  debugprintF(" Sende um ");
  debugprintln(next_send_hour);
  debugflush();
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Datenstring erstellen
//----------------------------------------------------------------
void DatenString_erstellen() {

  uint16_t count = 0;
  aktuell = rtc.now();
  count = sprintf(DatenString, "%d/", aktuell.year());
  count += sprintf(DatenString + count, "%2.2d/", aktuell.month());
  count += sprintf(DatenString + count, "%2.2d ", aktuell.date());
  count += sprintf(DatenString + count, "%2.2d:", aktuell.hour());
  count += sprintf(DatenString + count, "%2.2d:", aktuell.minute());
  count += sprintf(DatenString + count, "%2.2d", aktuell.second());

  if (Anzahl_Sensoren_Gewicht > 1) {
    count = Wert_hinzufuegen(count, DS_Temp, 1, No_Val);             // Wägezellentemperatur
  }
  count = Wert_hinzufuegen(count, SensorTemp[Beute1], 1, No_Val);    // Stocktemperatur 1
  if (Anzahl_Sensoren_Gewicht > 1) {
    count = Wert_hinzufuegen(count, SensorTemp[Beute2], 1, No_Val);  // Stocktemperatur 2
  }
  if (Anzahl_Sensoren_Gewicht > 2) {
    count = Wert_hinzufuegen(count, SensorTemp[Beute3], 1, No_Val);  // Stocktemperatur 3
  }
  if (Anzahl_Sensoren_Gewicht > 3) {
    count = Wert_hinzufuegen(count, SensorTemp[Beute4], 1, No_Val);  // Stocktemperatur 4
  }
  if (Anzahl_Sensoren_Gewicht > 4) {
    count = Wert_hinzufuegen(count, SensorTemp[Beute5], 1, No_Val);  // Stocktemperatur 4
  }
  if (Anzahl_Sensoren_Gewicht > 5) {
    count = Wert_hinzufuegen(count, SensorTemp[Beute6], 1, No_Val);  // Stocktemperatur 4
  }

  count = Wert_hinzufuegen(count, SensorTemp[Aussenwerte], 1, No_Val);     // Außentemperatur

  count = Wert_hinzufuegen(count, SensorFeuchte[Beute1], 1, No_Value);     // Stockluftfeuchte 1
  if (Anzahl_Sensoren_Gewicht > 1) {
    count = Wert_hinzufuegen(count, SensorFeuchte[Beute2], 1, No_Value);   // Stockluftfeuchte 2
  }
  if (Anzahl_Sensoren_Gewicht > 2) {
    count = Wert_hinzufuegen(count, SensorFeuchte[Beute3], 1, No_Value);   // Stockluftfeuchte 3
  }
  if (Anzahl_Sensoren_Gewicht > 3) {
    count = Wert_hinzufuegen(count, SensorFeuchte[Beute4], 1, No_Value);   // Stockluftfeuchte 4
  }
  if (Anzahl_Sensoren_Gewicht > 4) {
    count = Wert_hinzufuegen(count, SensorFeuchte[Beute5], 1, No_Value);   // Stockluftfeuchte 4
  }
  if (Anzahl_Sensoren_Gewicht > 5) {
    count = Wert_hinzufuegen(count, SensorFeuchte[Beute6], 1, No_Value);   // Stockluftfeuchte 4
  }
  count = Wert_hinzufuegen(count, SensorFeuchte[Aussenwerte], 1, No_Value);  // Außenluftfeuchte
  count = Wert_hinzufuegen(count, Licht, 1, No_Value);                 // Licht
  count = Wert_hinzufuegen(count, Gewicht[0], 2, No_Value);            // Gewicht 1
  if (Anzahl_Sensoren_Gewicht > 1) {
    count = Wert_hinzufuegen(count, Gewicht[1], 2, No_Value);          // Gewicht 2
  }
  if (Anzahl_Sensoren_Gewicht > 2) {
    count = Wert_hinzufuegen(count, Gewicht[2], 2, No_Value);          // Gewicht 3
  }
  if (Anzahl_Sensoren_Gewicht > 3) {
    count = Wert_hinzufuegen(count, Gewicht[3], 2, No_Value);          // Gewicht 4
  }
  if (Anzahl_Sensoren_Gewicht > 4) {
    count = Wert_hinzufuegen(count, Gewicht[4], 2, No_Value);          // Gewicht 5
  }
  if (Anzahl_Sensoren_Gewicht > 5) {
    count = Wert_hinzufuegen(count, Gewicht[5], 2, No_Value);          // Gewicht 6
  }
  count = Wert_hinzufuegen(count, Batteriespannung, 2, No_Value); // Akkuspannung
  count = Wert_hinzufuegen(count, Solarspannung, 2, No_Value);    // Solarspannung
  count = Wert_hinzufuegen(count, Service, 2, No_Value);    // Service
  count = Wert_hinzufuegen(count, Aux[1], 1, No_Value);  // Aux 1
  count = Wert_hinzufuegen(count, Aux[2], 2, No_Value);  // Aux 2
  count = Wert_hinzufuegen(count, Aux[3], 2, No_Value);  // Aux 3
  DatenString[count] = 0;
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Wert hinzufügen
//----------------------------------------------------------------
uint16_t Wert_hinzufuegen(uint16_t count, float Wert, uint8_t Nachkommastellen, float Fehler) {
  char Konvertierung[16];
  uint16_t count_neu = count;

  if (Wert == Fehler) {
    count_neu += sprintf(DatenString + count, ",%s", "");
  } else {
    dtostrf(Wert, 1, Nachkommastellen, Konvertierung);
    count_neu += sprintf(DatenString + count, ",%s", Konvertierung);
  }
  return count_neu;
}
//----------------------------------------------------------------
