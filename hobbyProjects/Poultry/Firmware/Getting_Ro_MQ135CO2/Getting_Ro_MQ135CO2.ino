#include "MQ135.h"
MQ135 gasSensor = MQ135(A0);
void setup (){
Serial.begin (115200);
}
void loop() {
  float value = analogRead(A0);
  Serial.print ("raw = ");
  Serial.println (value);
  float zero = gasSensor.getRZero();
  Serial.print ("rzero: ");
  Serial.println (zero);
  float ppm = gasSensor.getPPM();
  Serial.print ("ppm: ");
  Serial.println (ppm);
  delay(1000);
}
