#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DHTTYPE DHT22
#define DHTPIN     4
#define nextButton 15
#define backButton 2
#define Battery    5
#define OutLoad1   25
#define OutLoad2   26
#define OutLoad3   27
#define OutLoad4   14
#define MQ7CO      1
#define MQ135CO2   2
#define MQ135Am    3
#define RLAm 20  //The value of resistor RL is 20K
#define mAm -0.404215 //Enter calculated Slope 
#define bAm 0.864883 //Enter calculated intercept
#define RoAm 1 //Enter found Ro value
#define RLCO2 22  //The value of resistor RL is 22K
#define mCO2 -0.501265 //Enter calculated Slope 
#define bCO2 0.703103 //Enter calculated intercept
#define RoCO2 1 //Enter found Ro value


//-------------------int--------------------//
int Battvalue;
int Battperct;
int displayCount = 1;
int count = 0;
int nxtButtonState = 1;
int bckButtonState = 1;
int WiFiperct;

//--------------------float-----------------//
float t, h; //temp humidity
float mCO = -0.6527; //Slope
float bCO = 1.30; //Y-Intercept
float R0CO = 3.50; //Sensor Resistance in fresh air from previous code

//--------------------long------------------//
unsigned long displayTimer = millis(), postTimer = millis();
unsigned long NBtime = 0, BBtime = 0;


//--------------------char-----------------//
const char* ssid_AP = "ESP32-Access-Point";
const char* password_AP = "123456789";
char *ssid_STA;
char *password_STA;
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
char *dtostrf(double val, signed char width, unsigned char prec, char *s);
char backFlag = 0, nextFlag = 0, stopFlag = 1, WiFiConnectFlag = 0;

//--------------------string--------------//
// Service API Key
String apiKey = "GDKZYY76S4G4HLQX";
String battPrcnt,simPrcnt,wifiPrcnt, hum, temp, HI, CO2, CO, Ammo;
String Humidity,Temperature,heatIndex,CO2gas,COgas,Ammonia;
String configParam, ssid, password, output1, output2, output3, output4;

//----------------Declaration---------------//

WiFiServer server(80); //wifi server with port 80
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1);
  pinMode(nextButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(Battery, INPUT);
  pinMode(MQ7CO, INPUT);
  pinMode(MQ135CO2, INPUT);
  pinMode(MQ135Am, INPUT);
  pinMode(OutLoad1, OUTPUT);
  pinMode(OutLoad2, OUTPUT);
  pinMode(OutLoad3, OUTPUT);
  pinMode(OutLoad4, OUTPUT);



  WiFi.mode(WIFI_AP_STA); //set esp32 as AP and STA

  Serial.println("Setting AP (Access Point)…");
  WiFi.softAP(ssid_AP, password_AP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();

  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop()
{
  if (millis() - displayTimer > 250)
  {
    displayTimer = millis();
    getBatt();
    if(WiFiConnectFlag){
      WiFiSignal();
    }else{
      SIMsignal();
    }
    combineSensorData();
    showDisplay();
    clearString();
  }
  AP_mode();
  if (WiFiConnectFlag) {
    if (millis() - displayTimer > 300000) {
      combineSensorData();
      postWiFiData();
      clearString();
    }
    recvWiFiData();
  } else {
    if (millis() - displayTimer > 300000) {
      combineSensorData();
      postSIMData();
      clearString();
    }
    recvSIMData();
  }
}


void showDisplay() {
  if (displayCount)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BATT");
    display.setCursor(30, 0);
    display.println(battPrcnt);
    display.setCursor(75, 0);
    if(WiFiConnectFlag){
    display.println("WiFi");
    display.setCursor(100, 0);
    display.println(wifiPrcnt); 
    }else{
    display.println("SIM");
    display.setCursor(100, 0);
    display.println(simPrcnt);
    }

    display.setCursor(0, 15);
    display.println("Temperature");
    display.setCursor(78, 15);
    display.println(temp);
    display.setCursor(0, 30);
    display.println("Humidity");
    display.setCursor(78, 30);
    display.println(hum);
    display.setCursor(0, 45);
    display.println("Heat Index");
    display.setCursor(78, 45);
    display.println(HI);
    display.display();
  }
  else
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BATT");
    display.setCursor(30, 0);
    display.println(battPrcnt);
    display.setCursor(75, 0);
    if(WiFiConnectFlag){
    display.println("WiFi");
    display.setCursor(100, 0);
    display.println(wifiPrcnt); 
    }else{
    display.println("SIM");
    display.setCursor(100, 0);
    display.println(simPrcnt);
    }
    display.setCursor(0, 13);
    display.println("O2 in Air");
    display.setCursor(78, 13);
    display.println("22%");
    display.setCursor(0, 26);
    display.println("C02 in Air");
    display.setCursor(78, 26);
    display.println(CO2);
    display.setCursor(0, 39);
    display.println("C0 in Air");
    display.setCursor(78, 39);
    display.println(CO);
    display.setCursor(0, 52);
    display.println("Ammonia");
    display.setCursor(78, 52);
    display.println(Ammo);
    display.display();
  }
}

//----------------------buttons------------------//

void nxtBtn() {
  nxtButtonState = digitalRead(nextButton); //reading button status
  nextFlag = 1; //set Flag on button
  NBtime = millis(); //getting when button is pressed
}

void bckBtn() {
  bckButtonState = digitalRead(backButton); //reading button status
  backFlag = 1; //set Flag on button
  BBtime = millis(); //getting when button is pressed
}

void checkButtonpress() {
  if (nextFlag && (millis() - NBtime > 100)) {
    nextFlag = 0;
    !displayCount;
  }
  if (backFlag && (millis() - BBtime > 100)) {
    nextFlag = 0;
    !displayCount;
  }
}

void clearString() {
  temp = "";
  hum = "";
  HI = "";
  Temperature = "";
  Humidity = "";
  heatIndex = "";
  CO="";
  CO2="";
  Ammonia="";
  COgas="";
  CO2gas="";
  Ammo="";
  
}

void readDHT() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Humidity=String(h);
  Temperature=String(t);
  heatIndex=String(t);
  hum = String(h);
  hum += '%';
  temp = String(t);
  temp += 'C';
  HI = temp;
}

void readMQ135Am() {
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration
  float ratio; //Define variable for ratio

  VRL = analogRead(MQ135Am) * (3.3 / 4096); //Measure the voltage drop and convert to 0-5V
  Rs = ((3.3 * RLAm) / VRL) - RLAm; //Use formula to get Rs value
  ratio = Rs / RoAm; // find ratio Rs/Ro

  float ppm = pow(10, ((log10(ratio) - bAm) / mAm)); //use formula to calculate ppm

  Serial.print("NH3 (ppm) = "); //Display a ammonia in ppm
  Serial.print(ppm);   // set the cursor to column 0, line 1
  Serial.print("      Voltage = "); //Display a intro message
  Serial.println(VRL);
  Ammo+=String(ppm)+"ppm";
  Ammonia=String(ppm);
}

void readMQ135CO2() {
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration
  float ratio; //Define variable for ratio

  VRL = analogRead(MQ135CO2) * (3.3 / 4096); //Measure the voltage drop and convert to 0-5V
  Rs = ((3.3 * RLCO2) / VRL) - RLCO2; //Use formula to get Rs value
  ratio = Rs / RoCO2; // find ratio Rs/Ro

  float ppm = pow(10, ((log10(ratio) - bCO2) / mCO2)); //use formula to calculate ppm

  Serial.print("CO2 (ppm) = "); //Display a ammonia in ppm
  Serial.print(ppm);   // set the cursor to column 0, line 1
  Serial.print("      Voltage = "); //Display a intro message
  Serial.println(VRL);
  CO2+=String(ppm)+"ppm";
  CO2gas=String(ppm);
}

void readMQ7() {
  float sensor_volt; //Define variable for sensor voltage
  float RS_gas; //Define variable for sensor resistance
  float ratio; //Define variable for ratio
  int sensorValue = analogRead(MQ7CO); //Read analog values of sensor

  Serial.print("GAS SENSOR RAW VALUE = ");
  Serial.println(sensorValue);
  sensor_volt = sensorValue * (3.3 / 4096); //Convert analog values to voltage
  Serial.print("Sensor value in volts = ");
  Serial.println(sensor_volt);
  RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; //Get value of RS in a gas
  Serial.print("Rs value = ");
  Serial.println(RS_gas);
  ratio = RS_gas / R0CO; // Get ratio RS_gas/RS_air

  Serial.print("Ratio = ");
  Serial.println(ratio);
  double ppm_log = (log10(ratio) - bCO) / mCO; //Get ppm value in linear scale according to the the ratio value
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale
  Serial.print("Our desired PPM = ");
  Serial.println(ppm);
  double percentage = ppm / 10000; //Convert to percentage
  Serial.print("Value in Percentage = "); //Load screen buffer with percentage value
  Serial.println(percentage);
  CO+=String(ppm)+"ppm";
  COgas=String(ppm);
}

void combineSensorData() {
  readDHT();
  readMQ135Am();
  readMQ135CO2();
  readMQ7();
}

void postSIMData() {
  Serial.println("TCP Send :");
  Serial2.println("ATE1"); /* Check Communication */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT"); /* Check Communication */
  delay(1000);
  ShowSerialData(); /* Print response on the serial monitor */
  Serial2.println("AT+CREG?"); /* Network registration status */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT+CGATT?");  /* Attached to or detached from GPRS service */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT+CIPSHUT"); /* Deactivate GPRS PDP content */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT+CIPMUX=0");  /* Single TCP/IP connection mode */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CGATT=1"); /* Attach to GPRS Service */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CSTT=\"\",\"\",\"\""); /* Start task and set APN */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIICR"); /* Bring up wireless connection with GPRS */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIFSR"); /* Get local IP address */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();
  Serial2.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  /* Start up TCP connection */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIPSEND"); /* Send data through TCP connection */
  delay(2000);
  ShowSerialData();
  Serial2.println("GET https://api.thingspeak.com/update?api_key=GDKZYY76S4G4HLQX&field1=" + Temperature + "&field2=" + Humidity + "&field3=" + heatIndex + "&field4=" + CO2gas + "&field5=" + COgas + "&field6=" + Ammonia + "\r\n"); /* URL for data to be sent to */
  delay(5000);
  ShowSerialData();
  Serial2.println(char(26));
  delay(5000);
  Serial2.println();
  ShowSerialData();

  Serial.print("AT+CIPSHUT\\r\\n");
  Serial2.println("AT+CIPSHUT"); /* Deactivate GPRS PDP content */
  delay(1000);
  ShowSerialData();
}

void recvSIMData() {

}

void ShowSerialData()
{
  while (Serial2.available() != 0) /* If data is available on serial port */
    Serial.write(char (Serial2.read())); /* Print character received on to the serial monitor */
}

void postWiFiData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData1 = "api_key=" + apiKey + "&field1=" + Temperature + "&field2=" + Humidity + "&field3=" + heatIndex + "&field4=" + CO2gas + "&field5=" + COgas + "&field6=" + Ammonia;
    http.POST(httpRequestData1);
    http.end();
  } else {
    STA_config();
  }
}

void recvWiFiData() {

}

void getBatt() {
  Battvalue = 0;
  for (int i = 0; i < 10; i++)
  {
    Battvalue = Battvalue + analogRead(Battery);
    delay(1);
  }
  Battvalue = Battvalue / 10;
  if (Battvalue >= 3400 && Battvalue <= 4100)
  {
    Battperct = Battvalue - 4100;
    Battperct = Battperct + 700;
    Battperct = (Battperct * 100) / 700;
  }
  else if (Battvalue > 4100)
  {
    Battperct = 100;
  }
  else
  {
    Battperct = 0;
  }
  battPrcnt+=String(Battperct)+"%";
}

void SIMsignal() {


}

//-----------------------------------WiFi strength in percentage---------------------//

void WiFiSignal() {
  long rssi = WiFi.RSSI();
  rssi = -rssi;
  if (rssi == 0) {
  }
  else if (rssi < 27 && rssi != 0) {
    WiFiperct = 100;
  }
  else if (rssi >= 27 && rssi < 33) {
    WiFiperct = 150 - (5 / 2.7) * rssi;
  }
  else if (rssi >= 33 && rssi < 36) {
    WiFiperct = 150 - (5 / 3) * rssi;
  }
  else if (rssi >= 36 && rssi < 40) {
    WiFiperct = 150 - (5 / 3.3) * rssi;
  }
  else if (rssi >= 40 && rssi < 90) {
    WiFiperct = 150 - (5 / 3.5) * rssi;
  }
  else if (rssi >= 90 && rssi < 99) {
    WiFiperct = 150 - (5 / 3.3) * rssi;
  }
  wifiPrcnt+=String(WiFiperct)+"%";
}


void AP_mode() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read();
        currentLine += c;
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
      stopFlag = 1;
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
    configParam = currentLine;
    if (configParam != "") {
      parseData();
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

//--------------------parsing-----------------------------//
void parseData() {
  if (configParam[0] == '$') {
    int posHeader = configParam.indexOf('$');
    int posTail = configParam.indexOf('*');
    if (posHeader == 0)
    {
      int posComma1 = configParam.indexOf(',');
      ssid = configParam.substring((posComma1), posHeader + 1);
      password = configParam.substring((posTail), posComma1 + 1);

      int ssid_len = ssid.length() + 1;              //length of ssid string
      int pass_len = password.length() + 1;           //length of password string

      ssid_STA[ssid_len];                       //ssid as char[]
      ssid.toCharArray(ssid_STA, ssid_len);         //string to char array
      password_STA[pass_len];                    //password as char[]
      password.toCharArray(password_STA, pass_len);   //string to char array
      Serial.print("SSID STA : ");
      Serial.println(ssid_STA);
      Serial.print("Password STA : ");
      Serial.println(password_STA);
      configParam = "";
      STA_config();
    }
  }
  else
  {
    int posHeader = configParam.indexOf('@');
    int posTail = configParam.indexOf('*');
    if (posHeader == 0)
    {
      int posComma1 = configParam.indexOf(',');
      int posComma2 = configParam.indexOf(',' , posComma1 + 1);
      int posComma3 = configParam.indexOf(',' , posComma2 + 1);
      
      output1 = configParam.substring((posComma1), posHeader + 1);
      output2 = configParam.substring((posComma2), posComma1 + 1);
      output3 = configParam.substring((posComma3), posComma2 + 1);
      output4 = configParam.substring((posTail), posComma3 + 1);

      setOutput();
      configParam = "";
    }
  }
}

void setOutput(){
  if(output1){
    digitalWrite(OutLoad1,HIGH);
  }else{
    digitalWrite(OutLoad1,LOW);
  }
    if(output2){
    digitalWrite(OutLoad2,HIGH);
  }else{
    digitalWrite(OutLoad2,LOW);
  }
    if(output3){
    digitalWrite(OutLoad3,HIGH);
  }else{
    digitalWrite(OutLoad3,LOW);
  }
    if(output4){
    digitalWrite(OutLoad4,HIGH);
  }else{
    digitalWrite(OutLoad4,LOW);
  }
}

 
