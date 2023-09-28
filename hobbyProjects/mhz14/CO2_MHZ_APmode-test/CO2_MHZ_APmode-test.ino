#include "MHZ19.h"
#include <WiFi.h>
#include <WiFiAP.h>

const char *ssid = "ESP32_APmode";             
const char *pass = "123456789";
WiFiServer server(80);   

const int rx_pin = 13; //Serial rx pin no
const int tx_pin = 15; //Serial tx pin no

MHZ19 *mhz19_uart = new MHZ19(rx_pin,tx_pin);
String Co2;
void setup() {
    Serial.begin(115200);
    mhz19_uart->begin(rx_pin, tx_pin);
    Serial.println("Configuring access point...");

    // You can remove the password parameter if you want the AP to be open.
    WiFi.softAP(ssid, pass);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
  
    Serial.println("Server started");

}

void loop() {
  
  WiFiClient client = server.available();
  if (client) {
  while (client.connected()) {
  getCO2();
  client.println(Co2);
  Co2="";
  delay(1000);
      }
    client.stop();              
  }
}


void getCO2()
{
  measurement_t m = mhz19_uart->getMeasurement();
  Serial.print("co2: ");
  Serial.println(m.co2_ppm);
  Co2="CO2 in ppm = ";
  Co2+=String(m.co2_ppm);
}
