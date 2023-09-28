#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DHTPIN 4
#define DHTTYPE DHT22

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

unsigned long displayTimer=millis();
int displayCount=1;
int count=0;
float t,h; 
String battPrcnt="100%",simPrcnt="100%",hum,temp,heatIndex;


char *dtostrf(double val, signed char width, unsigned char prec, char *s);

void setup() {
  Serial.begin(9600);
   //dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() 
{
  if(millis()-displayTimer>5000)
  {
  displayTimer=millis();
  displayCount = !displayCount;
  if(displayCount)
  {
    readDHT();
    combineData();
  }
  else
      {
        clearString();
      }
  }
  showDisplay();
}


void showDisplay(){
  
  if(displayCount)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("BATT");
  display.setCursor(30,0);
  display.println(battPrcnt);
  display.setCursor(75,0);
  display.println("SIM");
  display.setCursor(100,0);
  display.println(simPrcnt);

  display.setCursor(0,15);
  display.println("Temperature");
  display.setCursor(78,15);
  display.println(temp);
  display.setCursor(0,30);
  display.println("Humidity");
  display.setCursor(78,30);
  display.println(hum);
  display.setCursor(0,45);
  display.println("Heat Index");
  display.setCursor(78,45);
  display.println(heatIndex);
  display.display();
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(45, 10);
    display.println("OTP");
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(20, 30);
    display.println("0345");
    display.display();
    
  }
}

void readDHT(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }
  hum =String(h);
  hum+='%';
  temp =String(t);
  temp+='C';
  heatIndex=temp;
}
void readMQ135(){
  
}

void readMQ7(){
  
}

void combineData(){
  
}
void postData(){
  
}

void getBatt(){
  
}
void getsingle(){
  
}
void clearString(){
 temp="";
 hum="";
 heatIndex=""; 
}
