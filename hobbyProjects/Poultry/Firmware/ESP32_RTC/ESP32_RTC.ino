
#include "RTClib.h"
String Time;
RTC_Millis rtc;

void setup () {
  Serial.begin(115200);
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
}

void loop () {
  TimeNow();
  Serial.println(Time);
  delay(1000);
}

void TimeNow() {
  DateTime now = rtc.now();
  Time = String(now.day())+ "/" + String(now.month()) + "/" + String(now.year()) + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}
