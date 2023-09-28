#include <Wire.h>                 //  12c protocol library
#include <WiFi.h>                 //  wifi library for esp32
#include <PubSubClient.h>         //  MQTT library
#include <Adafruit_Sensor.h>      //  Sensor library
#include <Adafruit_GFX.h>         //  Sensor library
#include <Adafruit_SSD1306.h>     //  Oled library
#include "DHT.h"                  //  DHT22 library

#define SCREEN_WIDTH 128          // OLED display width, in pixels
#define SCREEN_HEIGHT 64          // OLED display height, in pixels
#define DHTTYPE DHT22             // DHT sensor type we are using DHT22

#define DHTPIN     4              // Digital Pin for DHT22 sensor
const char* ssid = "Chili Garlic";              //  Network SSID
const char* password = "Mayonnaise200ml";       //  Nerwork Password

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);          // Temperature Sensor Pin and Type

WiFiClient espClient;              // WiFi Client
PubSubClient client(espClient);    // MQTT Client

const char* mqtt_server = "137.135.83.217";   //  MQTT Broker IP
int mqtt_port = 1883;                         //  MQTT Broker Port

float h, t;
String hum, temp;

void setup() {
  Serial.begin(115200);           //  Serial Started
  setup_wifi();                   //  Call Wifi Setup Function
  client.setServer(mqtt_server, mqtt_port);   //Setup MQTT Server

  dht.begin(); // begin dht temperature sensor
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {       // Address 0x3D for 128x64
    Serial.println("Failed to begin 12C_OLED Display");
    delay(1000);
  } else {
    display.clearDisplay();
    display.setTextSize(1);         //  text size can be selected from 1-5
    display.setTextColor(WHITE);
  }

}

void loop() {
  if (!client.connected()) {      //  Check client is connected or not
    reconnect();                  //  if not connected call reconnect function
  }
  client.loop();                  //  run Loop fuction while client is connected

  readDHT();                      //  Read Sensor values
  MQTTpublish();                  //  Publish Sensor data to MQTT Server
  showDisplay();                  //  Show Sensor data on OLED Display

  delay(3000);                    //  Delay function to wait for 3 seconds before getting new data

}

void showDisplay() {
  display.clearDisplay();                  // Clear previous display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 0);                // x,y cursor point on display of 128,64
  display.println("Temperature");
  display.setCursor(40, 35);
  display.println("Humidity");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30, 13);
  display.println(temp);
  display.setCursor(30, 48);
  display.println(hum);
  display.display();                     // Dispaly new values
}

void MQTTpublish() {
  char sendtemp[4];                     //  Character of 4 bytes as our range is 0-100
  sprintf(sendtemp, "%d", int(t));      //  Storing value in Char array
  char sendhum[4];                      //  Character of 4 bytes as our range is 0-100
  sprintf(sendhum, "%d", int(h));       //  Storing value in Char array
  Serial.print("Temperature : "); Serial.println(sendtemp);
  Serial.print("Humidity    : "); Serial.println(sendhum);
  Serial.println();
  client.publish("Esp32-DHT_Hum", sendhum);         //  Publishing Sensor value to Esp32-DHT_Hum Topic
  client.publish("Esp32-DHT_Temp", sendtemp);       //  Publishing Sensor value to Esp32-DHT_Temp Topic

}
void readDHT() {
  h = dht.readHumidity();               //  Getting Humidity
  t = dht.readTemperature();            //  Getting Temperature
  if (isnan(h) || isnan(t)) {           //  Checking if any value is NAN or not if yes then there is some issue with sensor
    Serial.println("Failed to read from DHT sensor!");
    return;                             //  Return to if satement until sensor works again
  }
  hum = ""; temp = "";                  //  Converting Float to String
  hum = String(h);
  hum += '%';
  temp = String(t);
  temp += 'C';
}


void setup_wifi() {
  WiFi.mode(WIFI_STA);          // Defining The mode of WIFI in our we only need station
  WiFi.begin(ssid, password);   // Setting WiFi

  while (WiFi.status() != WL_CONNECTED) {       //  Waiting until WiFi connects
    delay(500);
    Serial.print(".");
  }
   Serial.println("WiFi connected");
}


void reconnect() {
  while (!client.connected()) {                 // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);    // Create a random client ID
    if (client.connect(clientId.c_str())) {     // Attempt to connect
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);                              // Wait 3 seconds before retrying
    }
  }
}
