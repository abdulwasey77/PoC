#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define postTime 60000 // post API time in ms

const char* ssid     = "Chili Garlic";
const char* password = "Mayonnaise200ml";

const long frequency = 525E6;  // LoRa Frequency

const int csPin = 5;          // LoRa radio chip select
const int resetPin = 15;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

unsigned long postMillis;

int indx;
bool sendResp=false;
bool idFound = false;
String id,PostStr,parseData;
String sendID[10],sendTemp[10],sendHum[10],sendBatt[10];

String none = "NAN";
String idArray[10] = {"87565652505622622","875656525056221135","NAN","NAN","NAN","NAN","NAN","NAN","NAN","NAN"};

void setup() {
  Serial.begin(115200);                   // initialize serial
  LoRa.setPins(csPin, resetPin, irqPin);
  WiFi.begin(ssid,password);
  Serial.print("Connection To Wifi Router");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print("..");
    delay(300); 
  }
  Serial.println();
  Serial.println("Connected to Wifi Router");

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

   for(int i=0; i<10 ; i++){
   sendID[i]=none;
   sendTemp[i]=none;
   sendHum[i]=none;
   sendBatt[i]=none;
  }
  
  postMillis = millis();
}

void loop() {
  if(millis()-postMillis > postTime){
    postMillis = millis();
    LoRa_txMode();
    Serial.print("url :"); Serial.println(url_API());
    //send_Data();
    LoRa_rxMode();
  }
}

void Send_Data()
{
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client)
  {
    {
      HTTPClient https;
      Serial.print("postStr: ");
      Serial.println(PostStr);
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, PostStr)) //"https://www.barliquorcontrol.com/post.aspx?id=00:11:22:33:44:55&w=20.3"))
      {
        Serial.print("[HTTPS] GET...\n");
        int httpCode = https.GET();
        if (httpCode > 0)
        {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            String payload = https.getString();
            Serial.print("Payload: ");
            Serial.println(payload);
            parseData=payload;
           // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2";
          }
        }
        else
        {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      }
      else
      {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  }
  else
  {
    Serial.println("Unable to create client");
  }

  Serial.println();
}

String url_API(){
  String url="";
 
  for(int i=0; i<10 ; i++){
    //sendID[10],sendTemp[10],sendHum[10],sendBatt[10];
   url += "&d"+String(i+1)+"1="+sendID[i]; 
   url += "&d"+String(i+1)+"2="+sendTemp[i]; 
   url += "&d"+String(i+1)+"3="+sendHum[i];
   url += "&d"+String(i+1)+"3="+sendBatt[i];
   sendID[i]=none;
   sendTemp[i]=none;
   sendHum[i]=none;
   sendBatt[i]=none;
  }
  return url;
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
  int c1=message.indexOf(',');
  id = message.substring(c1,0);
  idFound = false;
  validID();
  
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
  for(int i=0; i<10 ; i++)
  {
    if(id==idArray[i]){
      Serial.print("id :"); Serial.print(id); Serial.print(" match at index :");  Serial.println(i);
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
