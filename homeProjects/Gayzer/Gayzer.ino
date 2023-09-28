#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define gayzer_button D1
const char* ssid = "Chili Garlic";
const char* password = "Mayonnaise200ml";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char datarec[40];
String datanew;
int counter=20,startFlag=1,millisFlag=1,timerFlag=0;
unsigned long timerMillis,timervalue; //1800000

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
   datarec[i]=payload[i];
  }
  Serial.println(datarec);
  datanew=String(datarec);
  Serial.print("String: "); Serial.println(datanew);
  if (datanew == "on") {
    if(startFlag){
    timervalue=millis();
    digitalWrite(gayzer_button, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.print("gayzer_button: ");
    Serial.println(datanew);
    startFlag=0;
    counter=20;
    timerFlag=1;
   char senddata[4];
   sprintf(senddata,"%d",counter);
   client.publish("gayzer_in",senddata);
    }
      for (int i = 0; i < 40; i++) {
   datarec[i]=0;
  }
  datanew="";
  }  
  if (datanew == "off") {
    timervalue=130000;
    startFlag=1;
    timerFlag=0;
    counter=0;
    digitalWrite(gayzer_button, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.print("gayzer_button: ");
    Serial.println(datanew);
   char senddata[4];
   sprintf(senddata,"%d",counter);
   client.publish("gayzer_in",senddata);
      for (int i = 0; i < 40; i++) {
   datarec[i]=0;
    }
    datanew="";
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      counter=0;
      char senddata[4];
      sprintf(senddata,"%d",counter);
      client.subscribe("gayzer_out");
      client.publish("gayzer_in",senddata);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(gayzer_button,OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
   if(timerFlag){
   if(millisFlag){
    millisFlag=0;
    timerMillis=millis();
   }
   if(millis()-timerMillis>=60000){
   counter--;
   millisFlag=1;
   char senddata[4];
   sprintf(senddata,"%d",counter);
   client.publish("gayzer_in",senddata);
   }
  } 
  if(millis()-timervalue>=1200000){
    startFlag=1;
    timerFlag=0;
    digitalWrite(gayzer_button, LOW);
    counter=0;
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
