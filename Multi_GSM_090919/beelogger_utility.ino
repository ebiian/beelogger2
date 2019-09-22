#include "beelogger_config.h"

//----------------------------------------------------------------
// Funktion Spannungen messen
//----------------------------------------------------------------
void Spannungen_messen() {
  Batteriespannung = Messe_Spannung(Batterie_messen);

  debugprintF(" Batterie [V]: ");
  debugprintln(Batteriespannung);
  debugflush();

  if (Batteriespannung > VMinimum) {
    Solarspannung = Messe_Spannung(Solarzelle_messen);

    debugprintF(" Solarspannung [V]: ");
    debugprintln(Solarspannung);
    debugflush();

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
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion Alarm konfigurieren
//----------------------------------------------------------------
void Alarm_konfigurieren() {
  int IntervallMinuten, tt;
  aktuell = rtc.now();
  uint8_t tx = aktuell.second();
  tt = (int)tx;
  tt -= (int)time_on;
  if (tt < 0) {
    tt += 60;
  }

  debugprintF("An-Zeit: ");
  debugprintln(tt);
  debugflush();

  Service = (float)tt;

  if (Batteriespannung > VAlternativ) {
    IntervallMinuten = WeckIntervallMinuten;
  } else {
    IntervallMinuten = AlternativIntervallMinuten;
  }
  long l_tm = aktuell.get();
  aktuell = l_tm + IntervallMinuten * 60; // xx Minuten später
#define runde_ 3
  tx = aktuell.minute();
  if (IntervallMinuten > runde_) {
    tx = tx - aktuell.minute() % runde_;  // Auf Minuten runden
  }
  rtc.enableInterrupts(aktuell.hour(), tx, 0); //interrupt at  hour, minute, second

  debugprintF("Wakeup at: ");
  debugprint(aktuell.hour());
  debugprint(":");
  debugprint(tx);
  debugprintln(":00");
  debugflush();

}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion user_Int
//   teste ob Schalter an D2 hat Interrupt gesetzt hat
//----------------------------------------------------------------
void user_Int() {  // User forced Interrupt
  LowPower.powerStandby(SLEEP_60MS, ADC_OFF, BOD_OFF);   // Schalter entprellen.
  if (digitalRead(DS3231_Interrupt_Pin) == false) {      // Schalter "ein" ?
    LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF);
#ifdef _EEProm_Satz  // EE-Prom-Version
    if (digitalRead(DS3231_Interrupt_Pin) == false) {
      send_data = 1;
    }
#else                // normaler beelogger
    while (digitalRead(DS3231_Interrupt_Pin) == false) {
      LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF);  // Warte auf Freigabe
    }
#endif
  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion SleepNow
//----------------------------------------------------------------
void SleepNow() {
  
  delay(5);
  hx711_SCK_Low();    // code order, important!
  
  digitalWrite(Power_Pin, LOW);
  delay(5);


  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);

  digitalWrite (ONE_WIRE_BUS, LOW);
  digitalWrite (DHT_Sensor_Pin[0], LOW);
  digitalWrite (DHT_Sensor_Pin[1], LOW);
  dfue_rxtx_off();

  attachInterrupt(0, WakeUp, LOW);  // D2 = Interrupt
  delay(1);

  if (ok_sleep) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  hx711_SCK_High();
  digitalWrite (DHT_Sensor_Pin[0], HIGH);
  digitalWrite (DHT_Sensor_Pin[1], HIGH);

  digitalWrite(Power_Pin, HIGH);
  delay (5);

  rtc.begin();
  rtc.clearINTStatus();
  aktuell = rtc.now();
  time_on = aktuell.second();

  user_Int();
  ok_sleep = true;

  delay(5);
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion WakeUp
//----------------------------------------------------------------
void WakeUp() {
  ok_sleep = false;
  detachInterrupt(0);
}
//----------------------------------------------------------------

//----------------------------------------------------------------
// Funktion freeRam
//----------------------------------------------------------------
int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
//----------------------------------------------------------------
