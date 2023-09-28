#define water_pin 13
void setup()
{
  Serial.begin(115200); 
  Serial.println("/** Water pressure sensor demo **/");
}
void loop()
{
  float value=0;
  for(int i=0; i<20 ;i++){
  value += analogRead(water_pin); 
  delay(1);
  }
  value/=20;
  float  voltage=(value*3.3)/4096;
  Serial.print("Analog Value  :   ");  Serial.println(value);
  Serial.print("Sensor Voltage:   ");  Serial.println(voltage);
  delay(3000);
}
