#include <WiFi.h>

#define LDRpin 33
String ldr;
//const char* ssid     = "Chili Garlic";
//const char* password = "Mayonnaise200ml";
const char* ssid     = "I-Tech-Router";
const char* password = "Mayonnaise200ml";
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(4,OUTPUT);

//  WiFi.mode(WIFI_AP_STA);
//  Serial.println("Setting AP (Access Point)…");
//  WiFi.softAP("esp32_Ap", "123456789");
//  IPAddress IP = WiFi.softAPIP();
//  Serial.print("AP IP address: ");
//  Serial.println(IP);
//
//  Serial.print("connecting to...");
//  Serial.println(ssid);
//
//  WiFi.begin(ssid, password);
//
//  for (int tr = 0; tr <= 20; tr++)
//  {
//    if (WiFi.status() != WL_CONNECTED)
//    {
//      Serial.print("...");
//      delay(300);
//    }
//    else
//    {
//      Serial.println("");
//      Serial.println("WiFi connected");
//      Serial.println("IP address: ");
//      Serial.println(WiFi.localIP());
//
//      break;
//    }
//  }
}

void loop() {
  // put your main code here, to run repeatedly:
LDR();
delay(2000);

}

void LDR()
{
  int photocellReading=0;
  for(int i=0; i<10; i++){
  delay(1);
  photocellReading = photocellReading+analogRead(LDRpin);
  }
  photocellReading=photocellReading/10;
  Serial.print("photocellReading: ");  Serial.println(photocellReading);
 if(photocellReading <=  2700)
  { 
    ldr = "Light";
    digitalWrite(4,LOW);
    Serial.println(ldr);
  }
  else
  {
    ldr ="Dark";
    digitalWrite(4,HIGH);
    Serial.println(ldr);
  }
}
