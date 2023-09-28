#include <SPI.h>              // include libraries
#include <LoRa.h>

const long frequency = 525E6;  // LoRa Frequency

const int csPin = 5;          // LoRa radio chip select
const int resetPin = 15;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

int indx;
bool sendResp=false;
bool idFound = false;
String id;
String sendID[10],sendTemp[10],sendHum[10],sendBatt[10];

String none[10]={"NAN","NAN","NAN","NAN","NAN","NAN","NAN","NAN","NAN","NAN"};
String idArray[10]={"87565652505622622","875656525056221135","NAN","NAN","NAN","NAN","NAN","NAN","NAN","NAN"};
void setup() {
  Serial.begin(115200);                   // initialize serial
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Gateway");
  Serial.println("Only receive messages from nodes");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {
}

void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
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

  Serial.print("Gateway Receive: ");
  Serial.println(message);

  int c1=message.indexOf(',');
  id = message.substring(c1,0);
  Serial.print("id: "); Serial.println(id);
   idFound = false;
 //  Serial.println("going in validID");
   validID();
 //  Serial.println("out of validID");
 //Serial.print("idFound : ");  Serial.println(idFound);
  if(idFound)
  {
    LoRa_sendMessage(id);
    Serial.print("message sent :");  Serial.println(id);
    sendID[indx]=id;
    int c2=message.indexOf(',',c1+1);
    String temp =message.substring(c2,c1+1);
    Serial.print("temp: "); Serial.println(temp);
    int c3=message.indexOf(',',c2+1);
    String hum =message.substring(c3,c2+1);
    Serial.print("hum: "); Serial.println(hum);
    int c4=message.indexOf('*',c4+1);
    String batt =message.substring(c4,c3+1);
    Serial.print("batt: "); Serial.println(batt);
    sendTemp[indx]=temp;
    sendHum[indx]=hum;
    sendBatt[indx]=batt;
  }  
}

void validID(){
 // Serial.println("in validID");
  for(int i=0; i<10 ; i++)
  {
   // Serial.print("id array[");  Serial.print(i); Serial.print("] : ");  Serial.println(idArray[i]);
    if(id==idArray[i]){
      Serial.print("id match at index : ");  Serial.println(i);
      indx = i;
      idFound = true;
    }
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
