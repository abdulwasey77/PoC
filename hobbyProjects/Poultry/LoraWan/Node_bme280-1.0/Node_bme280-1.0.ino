#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoUniqueID.h>

//define bmeSend true is using bme280 sensor with node otherwise false for random values
#define bmeSend false

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

const long frequency = 525E6;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

unsigned long sendTime = 20000; //20sec or 20000miliseconds set it according to your own requirement

float temp,hum,pres;

bool  startTrans=true;
bool  stopTrans=false;

unsigned long stopMillis;

String id;

void setup() {
  Serial.begin(115200);                   // initialize serial
  bme.begin(0x76);
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");
  Serial.println();
  for (size_t i = 0; i < UniqueIDsize; i++)
  {
    id+=String(UniqueID[i]);  
  }
  Serial.print("id :"); Serial.println(id);

  Serial.println("LoRa Node");
  Serial.println("Only receive messages from gateway");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {
  if (stopTrans) 
  {
    stopTrans = false;
    stopMillis = millis();
  }

  if(millis()-stopMillis>=sendTime && !startTrans){
    startTrans=true;
  }
  
  if(startTrans && runEvery(200))
  {
    String message = id;
    if(bmeSend){
    getvalue();
    message += ",";
    message += String(temp);
    message += ",";
    message += String(hum);
    message += ",";
    message += String(random(70,100));
    message += "*";
    }
    else
    {
    message += ",";
    message += String(random(10,50));
    message += ",";
    message += String(random(40,90));
    message += ",";
    message += String(random(70,100));
    message += "*";  
    }
    LoRa_sendMessage(message);
  }
}

void getvalue(){
    temp=bme.readTemperature();
  //  pres=bme.readPressure()/100.0;
    hum=bme.readHumidity();
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
  if(message==id)
  {
    startTrans=false;
    stopTrans=true;
  }
  
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
