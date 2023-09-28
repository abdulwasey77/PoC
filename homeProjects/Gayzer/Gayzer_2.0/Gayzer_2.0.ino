#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define gayzer_button D1
const char* ssid = "Chili Garlic";
const char* password = "Mayonnaise200ml";
const char* mqtt_server = "mqtt.eclipse.org";
String toint="1883";
int port =toint.toInt();

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char datarec[40];
String datanew;
int counter=0,startFlag=1,timerFlag=0;
unsigned long timerMillis;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  for(int tr=0;tr<=20;tr++)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("...");
      delay(500);
    }
    else
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
  }
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
  if (datanew == "increment") {
    Serial.print("gayzer_button: ");
    Serial.println(datanew);
    if(counter<=55){
    counter=counter+5;
    }
    char senddata[4];
    sprintf(senddata,"%d",counter);
    client.publish("gayzer_in",senddata);
    for (int i = 0; i < 40; i++) {
      datarec[i]=0;
  }
  datanew="";
  }
  if (datanew == "on") {
    if(startFlag){
    digitalWrite(gayzer_button, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.print("gayzer_button: ");
    Serial.println(datanew);
    startFlag=0;
    if(counter==0)
    {
    counter=20;
    }
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
      client.subscribe("gayzer_out");
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
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {
   if(timerFlag){
   if(millis()>timerMillis){
   char senddata[4];
   sprintf(senddata,"%d",counter);
   client.publish("gayzer_in",senddata);
   counter--;
   timerMillis=millis()+60000;
   }
  } 
  if(counter==0 && timerFlag==1){
    startFlag=1;
    timerFlag=0;
    digitalWrite(gayzer_button, LOW);
  }
  if (!client.connected()) {
    setup_wifi();
    reconnect();
  }
  client.loop();
}
