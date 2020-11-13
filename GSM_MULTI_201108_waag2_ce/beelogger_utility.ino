#include "beelogger_config.h"
//04.04.2020 aktuelle Zeit holen vor Alarm setzen
//25.04.2020 User Service
//03.05.2020 User Service Schalter
//14.07.2020 Servicewert minimal 1
//13.08.2020 clear rtc-int in User int
//----------------------------------------------------------------
// Funktion Spannungen messen
//----------------------------------------------------------------
void Spannungen_messen() {
  Batteriespannung = Messe_Spannung(Batterie_messen);

  debugprintF("Bat. [V]: ");
  debugprintln(Batteriespannung);
  debugflush();

  if (Batteriespannung > VMinimum) {
    Solarspannung = Messe_Spannung(Solarzelle_messen);

    debugprintF("Sol. [V]: ");
    debugprintln(Solarspannung);
    debugflush();

  }
}

float Messe_Spannung (uint8_t Pin) {
  int Messung_Spannung;
  float Spannung;
  Messung_Spannung = analogRead(Pin);
  Messung_Spannung = 0;
  for (uint8_t j = 0 ; j < 16; j++) {
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
  int IntervallMinuten;

  if (Batteriespannung > VAlternativ) {
    IntervallMinuten = WeckIntervallMinuten;
  } else {
    IntervallMinuten = AlternativIntervallMinuten;
  }
  Get_Time_On();
  long l_tm = aktuell.get();
  aktuell = l_tm + IntervallMinuten * 60; // xx Minuten später

  rtc.enableInterrupts(aktuell.hour(), aktuell.minute(), 0); //interrupt at  hour, minute, second

  debugprintF("Wakeup at: ");
  debugprint(aktuell.hour());
  debugprint(":");
#if myDEBUG
  if (aktuell.minute() < 10) debugprint("0");
#endif
  debugprintln(aktuell.minute());
  debugflush();
}
//----------------------------------------------------------------

//----------------------------------------------------------------
// Funktion Get_Time_On
//----------------------------------------------------------------
void Get_Time_On() {
  aktuell = rtc.now();
  uint32_t tt = aktuell.get() - time_on;
  if (tt == 0) tt = 1; // minimal 1 zur Unterscheidung von Reset
  Service = (float)tt;
  delay(5);
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion SleepNow
//----------------------------------------------------------------
void SleepNow() {

  attachInterrupt(0, WakeUp, LOW);  // D2 = Interrupt
  delay(1);

  while (ok_sleep) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    delay(100);  // Schalter entprellen.
  }

  ok_sleep = true;
  delay(5);
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion System_Off
//----------------------------------------------------------------
void System_Off() {
  hx711_SCK_Low();    // code order, important!
  digitalWrite(Power_Pin, LOW);
  delay(5);

  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);

  digitalWrite (ONE_WIRE_BUS, LOW);
  digitalWrite (DHT_Sensor_Pin[0], LOW);
  digitalWrite (DHT_Sensor_Pin[1], LOW);
}
//----------------------------------------------------------------

//----------------------------------------------------------------
// Funktion System_On
//----------------------------------------------------------------
void System_On() {
  hx711_SCK_High();
  digitalWrite (DHT_Sensor_Pin[0], HIGH);
  digitalWrite (DHT_Sensor_Pin[1], HIGH);

  digitalWrite(Power_Pin, HIGH);
  delay(5);           // vcc stable

  rtc.begin();
  rtc.clearINTStatus();
  delay(5);            // wait clear Int
  aktuell = rtc.now();
  time_on = aktuell.get();
  // User Service, force send data
  if (digitalRead(DS3231_Interrupt_Pin) == false) {      // switch on?
    LowPower.powerStandby(SLEEP_2S, ADC_OFF, BOD_OFF);
    if (digitalRead(DS3231_Interrupt_Pin) == false) {    // still on?
      nr_to_measure = 1;
    }
  }
}
//----------------------------------------------------------------


//----------------------------------------------------------------
// Funktion User_Int
//   teste ob Schalter an D2 hat Interrupt gesetzt hat
//----------------------------------------------------------------
uint8_t User_Int() {  // User forced Interrupt
  uint8_t ret_val = 0;
  int i = 0;
  uint32_t time_on_sav = time_on;

  rtc.clearINTStatus();
  delay(5);            // wait clear Int
  if (digitalRead(DS3231_Interrupt_Pin) == false) {      // Schalter "ein" ?
    debugprintlnF("User Service");
    debugflush();
    System_Off();
    debugend();
    Serial_rxtx_off();
    do {
      i++;
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  // Warte auf Freigabe
    } while ((digitalRead(DS3231_Interrupt_Pin) == false) && (i < 256));
    ret_val = 1;
    nr_to_measure = 1;      // force next send
    System_On();
    time_on = time_on_sav;
    Get_Time_On();
    System_On();           // Set Time On
    debugbegin(Serial_Baudrate);
  }
  return (ret_val);
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
