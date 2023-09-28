#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FS.h>

#define gayzer_button D1
//const char* ssid = "Chili Garlic";
//const char* password = "Mayonnaise200ml";

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (50)
char stopFlag = 1, WiFiConnectFlag = 0 , readfileFlag = 0, gayzerOnFlag = 0;
char msg[MSG_BUFFER_SIZE];
char datarec[40];
const char* ssid_AP = "Gayzer";
const char* password_AP = "123456789";
const char* ssid_STA;
const char* password_STA;
const char* mqtt_server;
int mqtt_port;
String datanew;
String configParam = "", ssid, password, serverMQTT, portMQTT;
int counter = 0, startFlag = 1, timerFlag = 0;
unsigned long timerMillis;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(gayzer_button, OUTPUT);
  bool success = SPIFFS.begin();
  if (!success) {
    Serial.println("Error mounting the file system");
    return;
  }
  readfile();
  setup_wifi();
}

void loop() {
  if (readfileFlag) {
    readfileFlag = 0;
    parseData();
    STA_config();
    MQTT_config();
  }
  AP_mode();
  if (timerFlag) {
    if (millis() > timerMillis) {
      char senddata[4];
      sprintf(senddata, "%d", counter);
      client.publish("gayzer_in", senddata);
      counter--;
      timerMillis = millis() + 60000;
    }
  }
  if (counter == 0 && timerFlag == 1) {
    startFlag = 1;
    timerFlag = 0;
    digitalWrite(gayzer_button, LOW);
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}



void setup_wifi() {
  WiFi.mode(WIFI_AP_STA); //set esp32 as AP and STA
  Serial.println("Setting AP (Access Point)…");
  WiFi.softAP(ssid_AP, password_AP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    datarec[i] = payload[i];
  }
  Serial.println(datarec);
  datanew = String(datarec);
  Serial.print("String: "); Serial.println(datanew);
  if (datanew == "increment") {
    Serial.print("gayzer_button: ");
    Serial.println(datanew);
    if (counter <= 55) {
      counter = counter + 5;
    }
    char senddata[4];
    sprintf(senddata, "%d", counter);
    client.publish("gayzer_in", senddata);
    for (int i = 0; i < 40; i++) {
      datarec[i] = 0;
    }
    datanew = "";
  }
  if (datanew == "on") {
    if (startFlag) {
      digitalWrite(gayzer_button, HIGH);   // Turn the LED on (Note that LOW is the voltage level
      Serial.print("gayzer_button: ");
      Serial.println(datanew);
      startFlag = 0;
      if (counter == 0)
      {
        counter = 20;
      }
      timerFlag = 1;
      char senddata[4];
      sprintf(senddata, "%d", counter);
      client.publish("gayzer_in", senddata);
    }
    for (int i = 0; i < 40; i++) {
      datarec[i] = 0;
    }
    datanew = "";
  }
  if (datanew == "off") {
    startFlag = 1;
    timerFlag = 0;
    counter = 0;
    digitalWrite(gayzer_button, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.print("gayzer_button: ");
    Serial.println(datanew);
    char senddata[4];
    sprintf(senddata, "%d", counter);
    client.publish("gayzer_in", senddata);
    for (int i = 0; i < 40; i++) {
      datarec[i] = 0;
    }
    datanew = "";
  }
}

void reconnect() {
  // Loop until we're reconnected
  for (int tr = 0; tr <= 10; tr++) {
    if (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Create a random client ID
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str())) {
        Serial.println("connected");
        client.subscribe("gayzer_out");
        break;
      } else {
        AP_mode();
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(1000);
      }
    }
  }
}

void AP_mode() {
  WiFiClient client = server.available(); // Listen for incoming clients
  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    configParam = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read();
        configParam += c;
        if (c == '*') {
          stopFlag = 0;
          client.stop();
          Serial.println("Client disconnected.");
          Serial.println("");
          break;
        }
      }
    }
    if (stopFlag) {
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
    stopFlag = 1;  
    
    if (configParam[0] == '$') {
      writefile();
      parseData();
      STA_config();
      MQTT_config();
    }
  }
}

void STA_config() {
  Serial.print("connecting to...");
  Serial.println(ssid_STA);

  WiFi.begin(ssid_STA, password_STA);

  for (int tr = 0; tr <= 20; tr++)
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
      WiFiConnectFlag = 1;
      break;
    }
  }
}

void MQTT_config() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

//--------------------parsing-----------------------------//
void parseData() {
  if (configParam[0] == '$') {
    int posHeader = configParam.indexOf('$');
    int posTail = configParam.indexOf('*');
    if (posHeader == 0)
    {
      int posComma1 = configParam.indexOf(',');
      int posComma2 = configParam.indexOf(',' , posComma1 + 1);
      int posComma3 = configParam.indexOf(',' , posComma2 + 1);

      ssid = configParam.substring((posComma1), posHeader + 1);
      password = configParam.substring((posComma2), posComma1 + 1);
      serverMQTT = configParam.substring((posComma3), posComma2 + 1);
      portMQTT = configParam.substring((posTail), posComma3 + 1);

      ssid_STA=ssid.c_str();
      password_STA=password.c_str();
      mqtt_server=serverMQTT.c_str();
      mqtt_port=portMQTT.toInt();

      Serial.print("SSID STA : ");
      Serial.println(ssid_STA);
      Serial.print("Password STA : ");
      Serial.println(password_STA);
      Serial.print("MQTT server : ");
      Serial.println(mqtt_server);
      Serial.print("MQTT port : ");
      Serial.println(mqtt_port);
      configParam = "";
    }
  }
}

void writefile() {
  File file = SPIFFS.open("/gayzer.txt", "w");

  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }

  int bytesWritten = file.print(configParam);

  if (bytesWritten == 0) {
    Serial.println("File write failed");
    return;
  }

  file.close();
}

void readfile() {
  File file2 = SPIFFS.open("/gayzer.txt", "r");

  if (!file2) {
    Serial.println("Failed to open file for reading");
    return;
  }
  configParam = "";
  Serial.println("File Content:");
  while (file2.available()) {

    char c = file2.read();
    configParam += c;
  }
  file2.close();
  Serial.print("configParam : "); Serial.println(configParam);
  if (configParam[0] == '$') {
    readfileFlag = 1;
  }
}

void removefile() {
  SPIFFS.remove("/gayzer.txt");
}
