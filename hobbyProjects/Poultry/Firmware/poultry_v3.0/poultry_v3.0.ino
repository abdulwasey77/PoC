#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DHTTYPE DHT22


#define DHTPIN     4
#define nextButton 15
#define backButton 5
#define Battery    13
#define OutLoad1   5
#define OutLoad2   18
#define OutLoad3   19
#define OutLoad4   23
#define MQ7CO      12
#define MQ135CO2   14
#define MQ135Am    27
#define C02RX      26
#define LDR        25


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
int httpCode;

//--------------------float-----------------//
float t, h; //temp humidity
float mCO = -0.6527; //Slope
float bCO = 1.30; //Y-Intercept
float R0CO = 3.50; //Sensor Resistance in fresh air from previous code

//--------------------long------------------//
unsigned long displayTimer = millis(), changeDisplay = millis(),timeOTP;
unsigned long NBtime = 0, BBtime = 0;
unsigned long min5 = millis(), sec10 = millis();
unsigned long postmillis;

//--------------------char-----------------//
const char* ssid_AP = "ESP32-Access-Point";
const char* password_AP = "123456789";
const char* ssid_STA;
const char* password_STA;
char stopFlag = 1, WiFiConnectFlag = 0;
char accessFlag = 0, refreshFlag = 0, simFlag = 1, otpFlag = 0;
char fail = 1;
char displayOTP,simReady=0;
char postAPI = 1, Action = 0, Resp = 0, postapiFlag = 0, actionFlag = 0, readresFlag = 0, readComp = 1;


//--------------------string--------------//
String battPrcnt, simPrcnt, wifiPrcnt, hum, temp, HI, CO2, CO, Ammo;
String Humidity, Temperature, heatIndex, CO2gas, COgas, Ammonia;
String configParam, ssid, password, output1, output2, output3, output4;
String PostStr;// = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2";
String parseData, readstring, accessToken, refreshToken;
String Key[10], Value[10], dataParse[10], Data, OTP, fans, ligths, foggers, splinkers, mode;
String mac;
//----------------Declaration---------------//

WiFiServer server(80); //wifi server with port 80
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);



//const char* ssid     = "Chili Garlic";
//const char* password = "Mayonnaise200ml";

//const char* ssid     = "I-Tech-Router";
//const char* password = "0512303189T";




void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  delay(1);
  Serial.setRxBufferSize(1024);
  Serial2.setRxBufferSize(1024);

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

  attachInterrupt(digitalPinToInterrupt(nextButton), nxtBtn, CHANGE);
  attachInterrupt(digitalPinToInterrupt(backButton), bckBtn, CHANGE);

  WiFi.mode(WIFI_AP_STA);
  Serial.println("Setting AP (Access Point)…");
  WiFi.softAP(ssid_AP, password_AP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();

  mac = WiFi.macAddress();
  //mac="30:AE:02:33:A4";
  Serial.print("Wifi Mac: ");
  Serial.println(mac);

  //PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac="+mac;
  // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/dataStoring?accessToken=MPfAaEeZfsyDEqa6mPhdmWvcn9k9R7yH1SztYkcvBmU7xVKNbeZvqUGkdDpfTzL6iekSUt7yWJyH3QkoDrHcP5PYwFDqTuc&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  STA_config();
  if (simFlag) {
    WiFiConnectFlag = 0;
    setupSIM900();
  }

}

void loop()
{
  AP_mode();
  checkButtonpress();
  if (millis() - displayTimer > 500) {
    displayTimer = millis();
    showDisplay();
  }
  if (millis() - changeDisplay > 30000) {
    changeDisplay = millis();
    displayCount = !displayCount;
  }
  if(otpFlag && millis()-timeOTP>60000){
    otpFlag=0;
    displayCount=0;
  }
  if (simReady) {
    postSim900API();
  }
  getToken();
  if (millis() - sec10 > 30000) {
    sec10 = millis();
    getData();
  }
  if (millis() - min5 > 300000) {
    min5=millis();
    postData();
  }
}


void setupSIM900() {
  sendATcommand("ATE1", 1000, 400);
  sendATcommand("AT", 1000, 400);
  sendATcommand("AT+CGATT=1", 1000, 400);
  sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000, 400);
  sendATcommand("AT+SAPBR=3,1,\"APN\",\"\"", 2000, 400);
  sendATcommand("AT+SAPBR=1,1", 2000, 1000);
  sendATcommand("AT+HTTPINIT", 1000, 400);
}

void postSim900API() {
  if (postAPI) {
    postAPI = 0;
    Serial2.println(PostStr);
    postmillis = millis();
    Serial.println("posting API");
    postapiFlag = 1;
  }
  if (postapiFlag == 1 && millis() - postmillis > 5000) {
    Responce();
    postapiFlag = 0;
    //Serial.println(readstring);
    Action = 1;
  }

  if (Action) {
    Action = 0;
    Serial2.println("AT+HTTPACTION=0");
    postmillis = millis();
    Serial.println("Action HTTP");
    actionFlag = 1;
  }
  if (actionFlag == 1 && millis() - postmillis > 10000) {
    Responce();
    actionFlag = 0;
    //Serial.println(readstring);
    Resp = 1;
  }

  if (Resp) {
    Resp = 0;
    Serial2.println("AT+HTTPREAD");
    postmillis = millis();
    Serial.println("Reading Responce");
    readresFlag = 1;
  }
  if (readresFlag == 1 && millis() - postmillis > 10000) {
    parseData = Responce();
    readresFlag = 0;
    Serial.print("Received Data : "); Serial.println(parseData);
    postAPI = 1;
    getSIM900String();
  }
}


void getSIM900String() {
  int sBrket = parseData.indexOf('{');
  int eBrket = parseData.indexOf('}');
  parseData = parseData.substring(eBrket + 1, sBrket);
  if (parseData == "") {
    Serial.println("parseData is empty");
    setupSIM900();
    postSim900API();
  } else {
    Serial.print("SIM900 Received Data : "); Serial.println(parseData);
    simReady = 0;
    parsingJSON();
  }

}

String sendATcommand(String toSend, unsigned long milliseconds, unsigned long breakTime ) {
  String result;
  Serial2.println(toSend);
  unsigned long readTime, startTime = millis();
  while (millis() - startTime < milliseconds) {
    if (Serial2.available()) {
      readTime = millis();
      char c = Serial2.read();
      result += c;
    } else {
      if (millis() - readTime > breakTime && result != "") {
        break;
      }
    }

  }
  return result;
}

String Responce() {
  String result;
  while (Serial2.available()) {
    char c = Serial2.read();
    result += c;
  }
  return result;
}

void postWifiAPI() {
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client)
  {
    {
      HTTPClient https;
      Serial.print("postStr: ");
      Serial.println(PostStr);
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, PostStr)) // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2";
      {
        checkButtonpress();
        Serial.print("[HTTPS] GET...\n");
        httpCode = https.GET();
        if (httpCode > 0)
        {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            String payload = https.getString();
            checkButtonpress();
            Serial.println(payload);
            parseData = payload;
            fail = 0;
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
    fail = 1;
  }

  Serial.println();
}

void parsingJSON() {
  int  lastcomma = 0;
  int  thiscomma = 0;
  int  len = parseData.length();

  int i = 0;
  String nameKey;
  String nameValue;
  while (thiscomma >= 0)
  {
    lastcomma = parseData.indexOf(',', thiscomma);
    thiscomma = parseData.indexOf(',', lastcomma + 1);
    if (i == 0) {
      dataParse[i] = parseData.substring(lastcomma, 1);
      Data = dataParse[i];
      int collon = Data.indexOf(':');
      int lenData = Data.length();
      Key[i] = Data.substring(collon, 0);
      Value[i] = Data.substring(lenData, collon + 1);
      if (Value[i][0] == '"') {
        int lenV = Value[i].length();
        Value[i] = Value[i].substring(1, lenV - 1);
      }
      nameKey = "Key[" + String(i) + "] :  ";
      nameValue = "       Value[" + String(i) + "] :  ";
      Serial.print(nameKey); Serial.print(Key[i]);
      Serial.print(nameValue); Serial.println(Value[i]);
      i++;
    }
    if (thiscomma >= 0)
    {
      dataParse[i] = parseData.substring(thiscomma, lastcomma + 1);
      Data = dataParse[i];
      int collon = Data.indexOf(':');
      int lenData = Data.length();
      Key[i] = Data.substring(collon, 0);
      Value[i] = Data.substring(lenData, collon + 1);
      if (Value[i][0] == '"') {
        int lenV = Value[i].length();
        Value[i] = Value[i].substring(1, lenV - 1);
      }
    }
    else
    {
      dataParse[i] = parseData.substring(len - 1, lastcomma + 1);
      Data = dataParse[i];
      int collon = Data.indexOf(':');
      int lenData = Data.length();
      Key[i] = Data.substring(collon, 0);
      Value[i] = Data.substring(lenData, collon + 1);
    }
    nameKey = "Key[" + String(i) + "] :  ";
    nameValue = "       Value[" + String(i) + "] :  ";
    Serial.print(nameKey); Serial.print(Key[i]);
    Serial.print(nameValue); Serial.println(Value[i]);
    if (Value[i][0] == '"') {
      int lenV = Value[i].length();
      Value[i] = Value[i].substring(1, lenV - 1);
    }
    i++;
  }
}

void getToken() {
  if (accessFlag) {
    if (simFlag && !simReady) {
      PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=" + mac + "\"";
      simReady = 1;
    }
    if (WiFiConnectFlag){
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=" + mac;
      postWifiAPI();
      parsingJSON();
    }
    if (Key[0] == "\"accessToken\"") {
      accessFlag = 0;
      refreshFlag=1;
      simReady=0;
      accessToken = Value[0];
      Serial.print("accessToken: "); Serial.println(accessToken);
    }
    if (Key[1] == "\"refreshToken\"") {
      refreshToken = Value[1];
      Serial.print("refreshToken: "); Serial.println(refreshToken);
    }
  }
  if (refreshFlag) {
    if (simFlag && !simReady) {
      PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?grantType=refreshToken&refreshToken=" + refreshToken + "\"";
      simReady = 1;
    }if (WiFiConnectFlag){
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?grantType=refreshToken&refreshToken=" + refreshToken;
      postWifiAPI();
      parsingJSON();
    }
    if (Key[0] == "\"accessToken\"") {
      refreshFlag = 0;
      simReady=0;
      accessToken = Value[0];
      Serial.print("accessToken: "); Serial.println(accessToken);
    }
    if (Key[1] == "\"refreshToken\"") {
      refreshToken = Value[1];
      Serial.print("refreshToken: "); Serial.println(refreshToken);
    }
  }
}

void postData() {
  if (simFlag) {
    PostStr = "";
    //PostStr = "http://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=MPfAaEeZfsSfb93XMnMRicdf6coiGuhBywNoDz79TJvo5zYuY21pSioRvRyAeop73Ri6nCiaHKPT6zm3sLssSvSoWQbGxG3&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=\"01/January/1970 00:00:12\"&mode=\"IT\"";
    PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=12/26/1970%2000:00:12&mode=IT\"";
    simReady = 1;

  } else {
    PostStr = "";
    // PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
    PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=12/26/1970%2000:00:12&mode=IT";
    postWifiAPI();
    if (httpCode == 400 || httpCode == 401 || httpCode == 403) {
      httpCode = 0;
      accessFlag = 1;
      refreshFlag = 1;
    } else {
      parsingJSON();
    }
  }
  if (Value[0] == "false") {
    accessFlag = 1;
  }
  if (Value[3] == "true") {
    accessFlag = 1;
  }
  if (Value[0] == "true") {
    refreshFlag = 0;
    if (Value[1] == "true" && Key[1]=="\"otp\"") {
      OTP = Value[2];
      displayCount = 2;
      timeOTP=millis();
      otpFlag=1;
      Serial.print("OTP: "); Serial.println(OTP);
    }
  }
}

void getData() {
  if (simFlag) {
    PostStr = "";
    //PostStr = "http://backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=MPfAaEeZfsSfb93XMnMRicdf6coiGuhBywNoDz79TJvo5zYuY21pSioRvRyAeop73Ri6nCiaHKPT6zm3sLssSvSoWQbGxG3&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z";
    PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z\"";
    simReady = 1;
  } else {
    PostStr = "";
    PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z";
    postWifiAPI();
    if (httpCode == 400 || httpCode == 401 || httpCode == 403) {
      accessFlag = 1;
      refreshFlag = 1;
    } else {
      parsingJSON();
    }
  }

  if (Value[0] == "false") {
    accessFlag = 1;
  }
  if (Value[0] == "true") {
    refreshFlag = 0;
    if (Key[1] != "\"msg\"") {
      fans = Value[1];
      Serial.print("fans: "); Serial.println(fans);
      ligths = Value[2];
      Serial.print("ligths: "); Serial.println(ligths);
      foggers = Value[3];
      Serial.print("foggers: "); Serial.println(foggers);
      splinkers = Value[4];
      Serial.print("splinkers: "); Serial.println(splinkers);
      mode = Value[6];
      Serial.print("mode: "); Serial.println(mode);
      if(Value[8]!="null"){
      OTP = Value[8];
      displayCount = 2;
      timeOTP=millis();
      otpFlag=1;
      Serial.print("OTP: "); Serial.println(OTP);
      }
    }
  }

}


void showDisplay() {
  if (displayCount == 2) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(45, 10);
    display.println("OTP");
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(20, 30);
    display.println(OTP);
    display.display();
  }
  if (displayCount)
  {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("BATT");
    display.setCursor(30, 0);
    display.println(battPrcnt);
    display.setCursor(75, 0);
    if (WiFiConnectFlag) {
      display.println("WiFi");
      display.setCursor(100, 0);
      display.println(wifiPrcnt);
    }
    if (simFlag) {
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
  if(!displayCount)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BATT");
    display.setCursor(30, 0);
    display.println(battPrcnt);
    display.setCursor(75, 0);
    if (WiFiConnectFlag) {
      display.println("WiFi");
      display.setCursor(100, 0);
      display.println(wifiPrcnt);
    } if (simFlag) {
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
  NBtime = millis(); //getting when button is pressed
}

void bckBtn() {
  bckButtonState = digitalRead(backButton); //reading button status
  BBtime = millis(); //getting when button is pressed
}

void checkButtonpress() {
  if (nxtButtonState == 0 && (millis() - NBtime >= 200)) {
    nxtButtonState = 1;
    Serial.print("bckD: "); Serial.println(bckButtonState);
    if (displayCount == 2) {
      displayCount=0;
    }
    displayCount = !displayCount;
  }
  if (bckButtonState == 0 && (millis() - BBtime >= 200)) {
    bckButtonState = 1;
    Serial.print("nxtD: "); Serial.println(nxtButtonState);
    if (displayCount == 2) {
      displayCount=0;
    }
    displayCount = !displayCount;
  }
}

void readDHT() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Humidity = String(h);
  Temperature = String(t);
  heatIndex = String(t);
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
  Ammo += String(ppm) + "ppm";
  Ammonia = String(ppm);
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
  CO2 += String(ppm) + "ppm";
  CO2gas = String(ppm);
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
  CO += String(ppm) + "ppm";
  COgas = String(ppm);
}

void combineSensorData() {
  readDHT();
  readMQ135Am();
  readMQ135CO2();
  readMQ7();
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
  battPrcnt += String(Battperct) + "%";
}

void SIMsignal() {
  simPrcnt = sendATcommand("AT+CSQ\r", 1000, 400);
  Serial.println(simPrcnt);
  int comma = simPrcnt.indexOf(',');
  int collon = simPrcnt.indexOf(':');
  simPrcnt = simPrcnt.substring(comma, collon + 2);
  int simSgnl = simPrcnt.toInt();
  if (simSgnl >= 0) {
    if (simSgnl < 32) {
      simSgnl = (simSgnl * 100) / 31;
      simPrcnt = String(simSgnl);
    } else {
      simPrcnt = '0';
    }
  } else {
    simPrcnt = '0';
  }
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
  wifiPrcnt = String(WiFiperct);
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
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
    stopFlag = 1;
    configParam = currentLine;
    if (configParam != "") {
      parseDataFunct();
    }
  }
}

void STA_config() {
  Serial.print("connecting to...");
  Serial.println(ssid_STA);

  WiFi.begin(ssid_STA, password_STA);

  for (int tr = 0; tr <= 30; tr++)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("...");
      delay(500);
      simFlag = 1;
    }
    else
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      WiFiConnectFlag = 1;
      simFlag = 0;
      break;
    }
  }
}

//--------------------parsing-----------------------------//
void parseDataFunct() {
  if (configParam[0] == '$') {
    int posHeader = configParam.indexOf('$');
    int posTail = configParam.indexOf('*');
    if (posHeader == 0)
    {
      int posComma1 = configParam.indexOf(',');
      ssid = configParam.substring((posComma1), posHeader + 1);
      password = configParam.substring((posTail), posComma1 + 1);

      ssid_STA=ssid.c_str();
      password_STA=password.c_str(); //string to char array
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

void setOutput() {
  if (output1) {
    digitalWrite(OutLoad1, HIGH);
  } else {
    digitalWrite(OutLoad1, LOW);
  }
  if (output2) {
    digitalWrite(OutLoad2, HIGH);
  } else {
    digitalWrite(OutLoad2, LOW);
  }
  if (output3) {
    digitalWrite(OutLoad3, HIGH);
  } else {
    digitalWrite(OutLoad3, LOW);
  }
  if (output4) {
    digitalWrite(OutLoad4, HIGH);
  } else {
    digitalWrite(OutLoad4, LOW);
  }
}
