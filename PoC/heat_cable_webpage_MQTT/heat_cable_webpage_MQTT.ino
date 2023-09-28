/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/
//#include <SPIFFS.h>
//#include <WiFi.h>
//include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266server.h>
//#include <WiFiAP.h>
//#include <WiFiClientSecure.h>
//#include <WiFiMulti.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FS.h>
#include <PubSubClient.h>
ESP8266WiFiMulti WiFiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

#define ONE_WIRE_BUS D2


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;


 char k;
//WiFiMulti WiFiMulti;
////////////////////////////////
String Argument_Name, Clients_Response1, Clients_Response2; 

String Device_Name;

////////////////////////
const char* mqtt_server = "35.158.43.238";
unsigned long lastMsg = 0;
char msg[50];
int value = 0;

String newmac;
String comp;
bool mac_higher = false;
bool no_remote=true;
bool clientmode=false,Wifi_Connected=false;
int count = 0;
int j = 0;
int n;
String ssid_w="";
String password_w="";
const char *ssid;
const char *mssid;
const char *password = "123456789";
float tempC;
ESP8266WebServer webserver(80);
WiFiServer server(8080);
WiFiClient espClient;
PubSubClient clientMqtt(espClient);
//////////////////////////////////////////////////////////
////////////////////////////////////////////////
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!clientMqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (clientMqtt.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     // client.publish("heatCable_pub", "hello world");
      // ... and resubscribe
      clientMqtt.subscribe("heatCable_sub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(clientMqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void storeSpiff(String message)
{
  appendFile(SPIFFS, "/tempdata.txt",message);
}
void ShowValues(){
String webpage;
webpage =  "<html>";
webpage += "<head><title>Heat Cable System</title>";
webpage += "<style>";
webpage += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: black;}";

webpage +="table, th, td {\n";
webpage +="border: 1px solid black;\n";
webpage +="border-collapse: collapse;\n";
webpage +="}\n";
webpage += "</style>";
webpage += "</head>";

webpage += "<body>";
webpage += "<h1><br>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbspHeat Cable System</h1>";  
String IPaddress = WiFi.localIP().toString();
webpage +="<table style=\"width:30%\">";
webpage += "<form action='http://192.168.4.1' method='POST'>";


////row 7
    webpage += "<tr>\n";
    webpage += "<td>Device ID</td>\n";
    webpage += "<td> <input type=\"text\" name=\"Device ID\" style=\"width: 100%;\" /> </td>\n";
    webpage += "</tr>\n";
    webpage += "<tr>\n";
    webpage += "<tr>\n";

////row 10


//   webpage += "<tr>\n";
//    webpage  +="<form action=\"get\">";
//   webpage += "<label for=\"Sensors1\">Choose a Sensor:</label>";
//   // webpage += "<select id=\"Sensors\"> type=\"select\">";
//  webpage += "<select id=\"Sensors\" name=\"Sensors\" type=\"select\">";
//    webpage += "<option value=\"1\">Front Shoulder</option>";
//    webpage += "<option value=\"2\">Back hips</option>";
//    webpage += "<option value=\"3\">Front left upper knee</option>";
//    webpage += "<option value=\"4\">Front left lower knee</option>";
//    webpage += "<option value=\"5\">Front right upper knee</option>";
//    webpage += "<option value=\"6\">Front right lower knee</option>";
//    webpage += "<option value=\"7\">Back right upper knee</option>";
//    webpage += "<option value=\"8\">Back right lower knee</option>";
//    webpage += "<option value=\"9\">Back right upper knee</option>";
//    webpage += "<option value=\"10\">Back left lower knee</option>";
//    webpage += "</select>";
//     //webpage += "<input type='submit' value='Submit'>";
//
//    webpage += "</tr>\n"; 
//
//
//
//webpage += "<form action=\"get\">";

////row 2
    webpage += "<tr>\n";
    webpage += "<td>SSID</td>\n";
    webpage += "<td> <input type=\"text\" name=\"SSID\" style=\"width: 100%;\" /> </td>\n";
    webpage += "</tr>\n"; 
     
////row 3
    webpage += "<tr>\n";
    webpage += "<td>Password</td>\n";
    webpage += "<td> <input type=\"text\" name=\"Password\" style=\"width: 100%;\" /> </td>\n";
    webpage += "</tr>\n"; 



////row 4
 webpage += "<tr>\n";
    webpage += "<td>ON_Temp</td>\n";
    webpage += "<td> <input type=\"text\" name=\"ON_Temp\" style=\"width: 100%;\" /> </td>\n";
    webpage += "</tr>\n";
 
///////row 5
 webpage += "<tr>\n";
    webpage += "<td>OFF_Temp</td>\n";
    webpage += "<td> <input type=\"text\" name=\"OFF_Temp\" style=\"width: 100%;\" /> </td>\n";
    webpage += "</tr>\n";
//  
////// row 6

 webpage += "<tr>\n";
    webpage += "<td>On_delay</td>\n";
    webpage += "<td> <input type=\"text\" name=\"On_delay\" style=\"width: 100%;\" /> </td>\n";
    webpage += "</tr>\n";
//  
///Row Submit
 webpage  +="<form action=\"/get\">";
   webpage += "</table>\n";
   webpage += "<input type='submit' value='Submit'>"; // omit <input type='submit' value='Enter'> for just 'enter'
    webpage += "</form>";
   webpage += "</body>";
  webpage += "</html>";
 
  
     

webserver.send(200, "text/html", webpage); // Send a response to the client asking for input
 
 if (webserver.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < webserver.args(); i++ ) {
 //     Serial.print(server.argName(i)); // Display the argument
      Argument_Name = webserver.argName(i);
      if (webserver.argName(i) == "Device ID") {
       
        Serial.print("");
        if((webserver.arg(i)!=0))
        {
        Device_Name=webserver.arg(i);
        
        }
        Serial.print("Device ID: ");
        Serial.println(webserver.arg(i));
       Clients_Response1 = webserver.arg(i);
      }
        if (webserver.argName(i) == "SSID") 
        {
        Serial.print("");
        if((webserver.arg(i)!=0))
        {
         ssid_w=webserver.arg(i);
        }
        Serial.print("SSID:");
        Serial.println(webserver.arg(i));
        
        Clients_Response2 =webserver.arg(i);
        
//        newwifisetting=1;
      }
      if (webserver.argName(i) == "Password") 
      {
        Serial.print("");
        if((webserver.arg(i)!=0))
        {
          password_w=webserver.arg(i);
        }
        Serial.print("Password:");
        Serial.println(webserver.arg(i));
        Clients_Response1 = webserver.arg(i);
        Connect_Wifi();
      }

      if (webserver.argName(i) == "ON_Temp") {
        Serial.print("");
        if((webserver.arg(i)!=0))
        {
          password_w=webserver.arg(i);
        }
        Serial.print("ON_Temp:");
        Serial.println(webserver.arg(i));
        Clients_Response1 = webserver.arg(i);
      }
      
      if (webserver.argName(i) == "OFF_Temp") {
        
        if((webserver.arg(i)!=0))
        {
          //config_data[3]=server.arg(i);
        }
        //Serial.print(",");
        Serial.print("OFF_Temp: ");
        Serial.println(webserver.arg(i));
        
        Clients_Response1 = webserver.arg(i);
//        sensor_pos=Clients_Response1;
      }

if (webserver.argName(i) == "On_delay") 
{
        
        if((webserver.arg(i)!=0))
        {
          //config_data[3]=server.arg(i);
        }
        //Serial.print(",");
        Serial.print("on_delay: ");
        Serial.println(webserver.arg(i));
        
        Clients_Response1 = webserver.arg(i);
//        sensor_pos=Clients_Response1;
      }

    }

      
}   
}

////////////////////////////////////////////////////////
void Connect_Wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_w,password_w);// connects to the WiFi router
  int tr=0;
  Serial.print("Trying to Connect :");Serial.println(ssid_w);
  for(tr=0;tr<=100;tr++)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("...");
      delay(500);
    }
    else
    {
      Serial.print("Connected To :");Serial.println(ssid_w);
      Wifi_Connected= true;
      clientMqtt.setServer(mqtt_server, 1883);
      clientMqtt.setCallback(callback);
      break;
    }
  }
  if(!Wifi_Connected)
  {
    WiFi.mode(WIFI_AP_STA);
  }
}
////////////////////////////////////////////////////
void newFile(fs::FS &fs, const char * path) {
  Serial.printf("new file: %s\r\n", path);

  //File file = fs.open(path, FILE_WRITE);
  File file = fs.open(path, "w");
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  file.close();
}
void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}
void appendFile(fs::FS &fs, const char * path, String message) {
  Serial.printf("Appending to file: %s\r\n", path);

  //File file = fs.open(path, FILE_APPEND);
  File file = fs.open(path, "a");
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.println(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}
void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  //File file = fs.open(path);
  File file = fs.open(path,"r");
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  bool check_flag=false;
  int line=0;
  while (file.available()) 
  {
    if(path=="/tempdata.txt")
    {
      comp = file.readStringUntil('\n');
      Serial.println(comp);
      comp. toCharArray(msg, comp.length());
      clientMqtt.publish("heatCable_pub",msg);
      line++;
      
    }
    else
    {
      comp = file.readStringUntil('\n');
      Serial.println("own mac:");
      Serial.println(newmac);
      Serial.println("other mac:");
      Serial.println(comp.substring(11));
      if (newmac < comp.substring(11)) 
      {
        check_flag=true;
      } 
      if (check_flag==false){mac_higher=true;}else{mac_higher=false;}
      Serial.println("mac higher:");
      Serial.println(mac_higher);
    }
  }
  if(line>=1)
  {
    deleteFile(SPIFFS,"/tempdata.txt");
    newFile(SPIFFS,"/tempdata.txt");
  }
  delay(2000);
}
void checkFile(fs::FS &fs, const char * path) 
{
  Serial.printf("Reading file: %s\r\n", path);
  
  File file = fs.open(path,"r");
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    Serial.println("Creating new file");
    newFile(SPIFFS,path);
    return;
  }
}
void setup() 
{
  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(5, OUTPUT);
  WiFi.mode(WIFI_AP_STA);
  Serial.println(WiFi.macAddress());
  String mac = WiFi.macAddress();
   
  sensors.begin();
  delay(100);
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  Serial.println("Device 0 Address: ");
  delay(1000);
  
  ////////////////////////////////////
  while (WiFi.macAddress()[j] != '\0') 
  {
    if (WiFi.macAddress()[j] == ':')
      j++;
    else {
      newmac = newmac + WiFi.macAddress()[j];
      j++;
    }
  }
  j = 0;
  ///////////////////////////
  Serial.println("string parse");
  Serial.println(newmac);
  String ss = "Heat_Cable""_" + newmac;
  //String macf=String(mac[0])+String(mac[1])+String(mac[3])+String(mac[4]);

  Serial.println("ssid :");
  Serial.println(ss);
  ssid = &ss[0];
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
  ///////////////////////////////////
  ////////////////////////////////

  bool success = SPIFFS.begin();
 
  if (success) 
  {
    Serial.println("File system mounted with success");
  } 
  else 
  {
    Serial.println("Error mounting the file system");
    return;
  }
  newFile(SPIFFS, "/data.txt");
  checkFile(SPIFFS, "/tempdata.txt");
}
void master_mode() {

  digitalWrite(14,HIGH);
  digitalWrite(5,HIGH);
  String ms = "Heat_Cable_AP";
  //String macf=String(mac[0])+String(mac[1])+String(mac[3])+String(mac[4]);
  WiFi.mode(WIFI_AP_STA);
 // WiFi.mode(WIFI_AP);
  Serial.println("master mode");
  Serial.println(ms);
  mssid = &ms[0];
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(mssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

 webserver.begin();
  Serial.println ( "HTTP server started" );
    // Next define what the server should do when a client connects
  webserver.on("/", ShowValues);


}
////////////////////////////////////////////////////
///////////////////////////////////////////////////
void client_mode() {

   digitalWrite(14,LOW);
  digitalWrite(5,LOW);
  String ms = "Heat_Cable_AP";
  //String macf=String(mac[0])+String(mac[1])+String(mac[3])+String(mac[4]);
 // WiFi.mode(WIFI_AP_STA);
 WiFi.mode(WIFI_STA);
  Serial.println("cleint mode");
  Serial.println(ms);
  mssid = &ms[0];
  // We start by connecting to a WiFi network
  WiFiMulti.addAP(mssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);



}

void check_Wifi()
{
 if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Wifi Disconnected");
      Wifi_Connected=false;
      mac_higher = false;
      no_remote=true;
      clientmode=false;
      count = 0;
    } 
}
void get_temp()
{
  sensors.requestTemperatures();
  printData(insideThermometer);
  delay(1000);
}
//////////////////////////////////////////////////
void loop() 
{
  check_Wifi();
  get_temp();
  while(!Wifi_Connected)
  {
    while (count == 0) 
    {
      Serial.println("scan start");
      // WiFi.scanNetworks will return the number of networks found
      for (k=0;k<5;k++)
      {
        n = WiFi.scanNetworks();
        delay(1000);
      }
      Serial.println("scan done");
      if (n == 0) 
      {
        Serial.println("no networks found");
      } 
      else 
      {
          Serial.print(n);
          Serial.println(" networks found");
          for (int i = 0; i < n; ++i) 
          {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            if(WiFi.SSID(i)=="Heat_Cable_AP")
            {
              Serial.println("already find AP");
              clientmode=true;
            }
  
            if (WiFi.SSID(i)[0] == 'H' && WiFi.SSID(i)[1] == 'e' && WiFi.SSID(i)[2] == 'a' && WiFi.SSID(i)[3] == 't')
            { 
              appendFile(SPIFFS, "/data.txt", &WiFi.SSID(i)[0]);
             no_remote=false;
            }
            delay(100);
    
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            delay(10);
        }
      }
      if(no_remote==true)
      {
        master_mode();
        mac_higher=true;
      }
      else
      {
        if(clientmode==true)
        {
          client_mode();
        }
        else
        {
          Serial.println("enter in compare mode");
          readFile(SPIFFS, "/data.txt");
          delay(1000);
          if (mac_higher) 
          {
            master_mode();
          }
          if (!mac_higher) 
          {
            client_mode();
          }
        }
      }
      count = 1;
    }
    ////////////////////////////////////////////////////////////////////////
    if (mac_higher) 
    {
      //Serial.println("Server started");
      webserver.handleClient();
      WiFiClient client = server.available();   // listen for incoming clients
  
      if (client) 
      {
        Serial.println("New Client.");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) 
        {
          if (client.available()) 
          {
            char c;
            c=client.read();// read a byte, then
            currentLine+=c;
           if(c=='\n')
           {
            Serial.println(currentLine);
            storeSpiff(currentLine);
           }
            
          }
          
        }
      }
  
    }
  
    ///////////////////////////////////////////////////////////////
    if (!mac_higher) 
    {
      const uint16_t port = 8080;
      const char * host = "192.168.4.1"; // ip or dns
  
      Serial.print("Connecting to ");
      Serial.println(host);
  
      // Use WiFiClient class to create TCP connections
      WiFiClient client;
  
      if (!client.connect(host, port)) 
      {
        Serial.println("Connection failed.");
        Serial.println("Waitingg 5 seconds before retrying...");
        delay(5000);
        return;
      }
  
      // This will send a request to the server
      //uncomment this line to send an arbitrary string to the server
      get_temp();
      String msg = String(tempC)+"_"+newmac;
      client.println(msg);
      //uncomment this line to send a basic document request to the server
      // client.print("GET /index.html HTTP/1.1\n\n");
  
      int maxloops = 0;
  
      //wait for the server's reply to become available
      while (!client.available() && maxloops < 1000)
      {
        maxloops++;
        delay(1); //delay 1 msec
      }
      if (client.available() > 0)
      {
        //read back one line from the server
        String line = client.readStringUntil('\r');
        Serial.println(line);
      }
      else
      {
        Serial.println("client.available() timed out ");
      }
  
      Serial.println("Closing connection.");
      client.stop();
  
      Serial.println("Waiting 5 seconds before restarting...");
      delay(5000);
  
    }
    /////////////////////////////////////////////////////
  }
  if(Wifi_Connected)
  {
     if (!clientMqtt.connected()) 
     {
      reconnect();
     }
     clientMqtt.loop();
     readFile(SPIFFS,"/tempdata.txt");
  }

}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.println(deviceAddress[i], HEX);
  }
}
void printTemperature(DeviceAddress deviceAddress)
{
  tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.println(tempC);
//  Serial.print(" Temp F: ");
//  Serial.print(DallasTemperature::toFahrenheit(tempC));
}
void printData(DeviceAddress deviceAddress)
{
  printTemperature(deviceAddress);
}
