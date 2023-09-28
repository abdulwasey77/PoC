#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "RTClib.h"
#include "DFRobot_OxygenSensor.h"
#include <Adafruit_BMP085.h>
#include "MHZ19.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DHTTYPE DHT22
#define COLLECT_NUMBER    10             // collect number, the collection range is 1-100.
#define Oxygen_IICAddress ADDRESS_3

#define DHTPIN     23
#define nextButton 5
#define backButton 15
#define OutLoad1   12
#define OutLoad2   4
#define OutLoad3   18
#define OutLoad4   19
#define MQ7CO      32  
#define CO2RX      26
#define MQ135CO2   34
#define MQ137Am    36  
#define LDRpin     35  
#define waterPin   33  

#define RLAm 22 
#define mAm -0.404215 
#define bAm 0.864883 
#define RoAm 5 

#define RL_VALUE_CO  5 
#define RL_VALUE_CO2 5 

//-------------------int--------------------//
int READ_SAMPLE_INTERVAL_CO=50;                      
int READ_SAMPLE_TIMES_CO=1;  
int READ_SAMPLE_INTERVAL_CO2=50;                      
int READ_SAMPLE_TIMES_CO2=1; 
int displayCount = 1;
int count = 0;
int nxtButtonState = 1;
int bckButtonState = 1;
int WiFiperct;
int httpCode;
int tryAgain=0;

//--------------------float-----------------//
float COCurve[3]  =  {1.77,0.20,-1.36};                                                                                                    
float RoCO        =  2.4;  
float CO2Curve[3]  =  {1,0.1,-0.36};                                                                                                    
float RoCO2        =  4;   
float oxygenData;
const float  OffSet = 0.483 ;
float vWater, pWater;
float t, h, hi; //temp humidity heatindex
long iPPM_CO = 0;
long iPPM_CO2 = 0;

//--------------------long------------------//
unsigned long displayTimer = millis(), changeDisplay = millis(), timeOTP;
unsigned long NBtime = 0, BBtime = 0;
unsigned long min5 = millis(), sec10 = millis();
unsigned long postmillis, dataGetTime, LastPT, showLPT;

//--------------------char-----------------//
const char* ssid_AP = "ESP32-Access-Point";
const char* password_AP = "123456789";
const char* ssid_STA="";//= "I-Tech-Router";
const char* password_STA;//= "0512303189T";
char stopFlag = 1, WiFiConnectFlag = 0;
char accessFlag = 0, refreshFlag = 0, simFlag = 1, otpFlag = 0, getFlag = 0, postFlag = 0;
char fail = 1;
char displayOTP, APFlag = 0;

//--------------------string--------------//
String battPrcnt, simPrcnt, wifiPrcnt, hum, temp, HI, CO2, CO, Ammo, O2, ldr,airP,waterP;
String Humidity, Temperature, heatIndex, CO2gas, COgas, Ammonia, O2gas,airPres,waterPres;
String configParam, ssid, password, output1, output2, output3, output4;
String PostStr;// = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2";
String parseData, readstring, accessToken = "", refreshToken;
String Key[10], Value[10], dataParse[10], Data, OTP, fans, ligths, foggers, splinkers, sendmode;
String mac, deviceDateTime;
String ErrorSim;
//----------------Declaration---------------//

WiFiServer server(80);            //wifi server with port 80
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);          //Temperature Sensor Pin and Type
DFRobot_OxygenSensor Oxygen;       //Oxygen Sensor
Adafruit_BMP085 bmp;               //Pressure Sensor
MHZ19 *mhz19_uart = new MHZ19(CO2RX, MQ135CO2);  //CO2 Sensor MHZ19 Usart Setting
RTC_Millis rtc;                    //Clock

//-----------------------------------------SETUP---------------------------------------//
void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  //mhz19_uart->begin(CO2RX,CO2TX);
  //delay(1);
  Serial.setRxBufferSize(1024);
  Serial2.setRxBufferSize(1024);        //Increaing Buffer size to 1024 bytes
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));    //Date and Time when Program burns into flash

  pinMode(nextButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(OutLoad1, OUTPUT);
  pinMode(OutLoad2, OUTPUT);
  pinMode(OutLoad3, OUTPUT);
  pinMode(OutLoad4, OUTPUT);

  WiFi.mode(WIFI_AP_STA);         //WiFi Mode 3 as AP and Station
  Serial.println("Setting AP (Access Point)…");
  WiFi.softAP(ssid_AP, password_AP);
  IPAddress IP = WiFi.softAPIP();  //IP address of AP mode
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();

  mac = WiFi.macAddress();
  //mac="30:AE:02:33:A4";
  Serial.print("Wifi Mac: ");
  Serial.println(mac);
  //
  //  //PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac="+mac;
  //  // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/dataStoring?accessToken=MPfAaEeZfsyDEqa6mPhdmWvcn9k9R7yH1SztYkcvBmU7xVKNbeZvqUGkdDpfTzL6iekSUt7yWJyH3QkoDrHcP5PYwFDqTuc&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
  dht.begin(); //begin dht temperature sensor
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println("Failed to begin 12C_OLED Display");
    delay(1000);
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
  }
  //  if(!Oxygen.begin(Oxygen_IICAddress)) {
  //  Serial.println("Failed to begin Oxygen Sensor");
  //  delay(1000);
  //  }
    if(!bmp.begin()) {
    Serial.println("Failed to begin BMP180 Sensor");
    delay(1000);
    }
  
  STA_config();
}

//-----------------------------------------LOOP---------------------------------------//
void loop()
{
  if (WiFi.status() != WL_CONNECTED && WiFiConnectFlag) {
    STA_config();
  }
  AP_mode();
  if(tryAgain>4){
    STA_config();
  }
  if (millis() - displayTimer > 2000) {
    displayTimer = millis();
    showDisplay();
    //combineSensorData();
  }
  if (millis() - changeDisplay > 20000) {
    changeDisplay = millis();
    displayCount++;
    if(displayCount>3)
    displayCount=1;
  }
  if (otpFlag && millis() - timeOTP > 10000) {
    otpFlag = 0;
    displayCount = 1;
  }
  if (accessToken == "") {
    accessFlag = 1;
    getToken();
  }
  getToken();
  if (millis() - sec10 > dataGetTime) {
    sec10 = millis();
    TimeNow();
    combineSensorData();
    if (WiFiConnectFlag) {
      WiFiSignal();
    }else{
     SIMsignal(); 
    }
    Serial.println();
    getData();
  }
  if (millis() - min5 > 300000) {
    min5 = millis();
    combineSensorData();
    TimeNow();
    Serial.println();
    postData();
    if(simFlag && ssid_STA!="")
    STA_config();
  }
}

//-----------------------------Getting Time Using Millis RTC---------------------------//
void TimeNow() {
  DateTime now = rtc.now(); //Getting Time
  deviceDateTime = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

//---------------------------------Setting up SIM900 Module-------------------------------//
void setupSIM900() {
  sendATcommand("ATE1", 1000, 400);
  sendATcommand("AT", 1000, 400);
  sendATcommand("AT+CGATT=1", 1000, 400);
  sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000, 400);
  sendATcommand("AT+SAPBR=3,1,\"APN\",\"\"", 2000, 400);
  sendATcommand("AT+SAPBR=1,1", 2000, 1000);
  sendATcommand("AT+HTTPINIT", 1000, 400);
}

//------------------------------Posting API using SIM900 Module----------------------------//
void postSim900API() {
  Serial.print("PostStr SIM900 : "); Serial.println(PostStr);
  readstring = sendATcommand(PostStr, 1000, 500);
  Serial.println(readstring);
  //
  readstring = sendATcommand("AT+HTTPACTION=0", 15000, 10000);
  Serial.println(readstring);
  //
  parseData = sendATcommand("AT+HTTPREAD", 1000, 500);
  Serial.print("readstring : "); Serial.print(readstring);

  Error();

  if (ErrorSim == "ERROR" || ErrorSim == "ROR") {
    Serial.println("SIM900 Failed to Post");
    STA_config();
  }
  getSIM900String();
}

//--------------------------Extracting JSON String from SIM900 Responce-------------------------------//
void getSIM900String() {
  int sBrket = parseData.indexOf('{');
  int eBrket = parseData.indexOf('}');
  parseData = parseData.substring(eBrket + 1, sBrket);
  if (parseData == "") {
    Serial.println("Noting Received Check SIM900");
    tryAgain++;
  } else {
    Serial.print("SIM900 Received Data : "); Serial.println(parseData);
    parsingJSON();
  }
}

//---------------------------------Send AT command Function-------------------------------//
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

//---------------------------------Reading SIM900 ERROR-------------------------------//
void Error() {
  int last = readstring.length();
  int E = readstring.indexOf('E', last - 10);
  ErrorSim = readstring.substring(E + 5, E);
  if(ErrorSim[0]=='R'){
    ErrorSim = ErrorSim.substring(3,0);
  }
  Serial.print("ErrorSim: "); Serial.println(ErrorSim);
}

//---------------------------------Setting up GSM Module-------------------------------//
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
        Serial.print("[HTTPS] GET...\n");
        httpCode = https.GET();
        if (httpCode > 0)
        {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            String payload = https.getString();
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

//-----------------------------Parsing JSON String into Keys and Values----------------------------//
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

//---------------------------------Getting Access and Refresh Tokens-------------------------------//
void getToken() {
  if (accessFlag) {
    if (simFlag) {
      PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=" + mac + "\"";
      postSim900API();
    }
    else if (WiFiConnectFlag) {
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=" + mac;
      postWifiAPI();
      parsingJSON();
    }
    if (Key[0] == "\"accessToken\"") {
      accessFlag = 0;
      accessToken = Value[0];
      Serial.print("accessToken: "); Serial.println(accessToken);
    }
    if (Key[1] == "\"refreshToken\"") {
      refreshToken = Value[1];
      Serial.print("refreshToken: "); Serial.println(refreshToken);
    }
  }
  if (refreshFlag) {
    if (simFlag) {
      PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?grantType=refreshToken&refreshToken=" + refreshToken + "\"";
      postSim900API();
    } else if (WiFiConnectFlag) {
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?grantType=refreshToken&refreshToken=" + refreshToken;
      postWifiAPI();
      parsingJSON();
    }
    if (Key[0] == "\"accessToken\"") {
      refreshFlag = 0;
      accessToken = Value[0];
      Serial.print("accessToken: "); Serial.println(accessToken);
    }
    if (Key[1] == "\"refreshToken\"") {
      refreshToken = Value[1];
      Serial.print("refreshToken: "); Serial.println(refreshToken);
    }
  }
}

//---------------------------------POST data to server-------------------------------//
void postData() {
  if (simFlag) {
    PostStr = "";
    TimeNow();
    //PostStr = "http://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=MPfAaEeZfsSfb93XMnMRicdf6coiGuhBywNoDz79TJvo5zYuY21pSioRvRyAeop73Ri6nCiaHKPT6zm3sLssSvSoWQbGxG3&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=\"01/January/1970 00:00:12\"&mode=\"IT\"";
    //PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=&humi=&heat=&co2g=&coga=&ammo=&o2ga=&airf=&ldrl=&deviceDateTime=&mode=IT\"";
    PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=" + Temperature + "&humi=" + Humidity + "&heat=" + heatIndex + "&co2g=" + CO2gas + "&coga=" + COgas + "&ammo=" + Ammonia + "&airf="+airP+"&ldrl=" + ldr + "&deviceDateTime=" + deviceDateTime + "&mode=" + sendmode + "\"";
    postSim900API();

  } else if (WiFiConnectFlag)
  {
    PostStr = "";
    // PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
    PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=" + Temperature + "&humi=" + Humidity + "&heat=" + heatIndex + "&co2g=" + CO2gas + "&coga=" + COgas + "&ammo=" + Ammonia + "&airf="+airP+"&ldrl=" + ldr + "&deviceDateTime=" + deviceDateTime + "&mode=" + sendmode;
    postWifiAPI();
    if (httpCode == 400 || httpCode == 401 || httpCode == 403) {
      httpCode = 0;
      accessFlag = 1;
      refreshFlag = 1;
    } else {
      parsingJSON();
    }
  }
  postAction();
}

void postAction() {
  if (Value[0] == "false") {
    Value[0]="";
    refreshFlag = 1;
  }
  if (Value[3] == "true") {
    Value[3]="";
    accessFlag = 1;
  }
  if (Value[0] == "true") {
    Value[0]="";
    refreshFlag = 0;
    LastPT = millis();
    if (Value[1] == "true" && Key[1] == "\"otp\"") {
      OTP = Value[2];
      timeOTP = millis();
      otpFlag = 1;
      Serial.print("OTP: "); Serial.println(OTP);
    }
  }

}

//---------------------------------Get Responce from server-------------------------------//
void getData() {
  if (simFlag) {
    //Serial.print("deviceDateTime: "); Serial.println(deviceDateTime);
    PostStr = "";
    //PostStr = "http://backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=MPfAaEeZfsSfb93XMnMRicdf6coiGuhBywNoDz79TJvo5zYuY21pSioRvRyAeop73Ri6nCiaHKPT6zm3sLssSvSoWQbGxG3&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z";
    //PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime="+deviceDateTime+"&mode="+sendmode+"&dateTime=2020-11-21T15:52:28.510Z\"";
    PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=" + fans + "&lights=" + ligths + "&foggers=" + foggers + "&splinkers=" + splinkers + "&deviceDateTime=" + deviceDateTime + "&mode=" + sendmode + "&dateTime=2020-11-21T15:52:28.510Z\"";
    postSim900API();
  } else if (WiFiConnectFlag) {
    PostStr = "";
    PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=" + fans + "&lights=" + ligths + "&foggers=" + foggers + "&splinkers=" + splinkers + "&deviceDateTime=" + deviceDateTime + "&mode=" + sendmode + "&dateTime=2020-11-21T15:52:28.510Z";
    postWifiAPI();
    if (httpCode == 400 || httpCode == 401 || httpCode == 403) {
      accessFlag = 1;
      refreshFlag = 1;
    } else {
      parsingJSON();
    }
  }
  getAction();
}


void getAction() {

  if (Value[0] == "false") {
    Value[0]="";
    refreshFlag = 1;
  }
  if (Value[0] == "true") {
    Value[0]="";
    if (!APFlag) {
      refreshFlag = 0;
      if (Key[1] != "\"msg\"") {
        fans = Value[1];
        Serial.print("fans: "); Serial.println(fans);

        if (fans == "true") {
          digitalWrite(OutLoad1, HIGH);
        } else {
          digitalWrite(OutLoad1, LOW);
        }
        ligths = Value[2];
        Serial.print("ligths: "); Serial.println(ligths);
        if (ligths == "true") {
          digitalWrite(OutLoad2, HIGH);
        } else {
          digitalWrite(OutLoad2, LOW);
        }
        foggers = Value[3];
        Serial.print("foggers: "); Serial.println(foggers);

        if (foggers == "true") {
          digitalWrite(OutLoad3, HIGH);
        } else {
          digitalWrite(OutLoad3, LOW);
        }
        splinkers = Value[4];
        Serial.print("splinkers: "); Serial.println(splinkers);

        if (splinkers == "true") {
          digitalWrite(OutLoad4, HIGH);
        } else {
          digitalWrite(OutLoad4, LOW);
        }

        sendmode = Value[6];
        Serial.print("mode: "); Serial.println(sendmode);
        if (Value[7] == "true" && Key[7] == "\"otp\"") {
          OTP = Value[8];
          timeOTP = millis();
          otpFlag = 1;
          Serial.print("OTP: "); Serial.println(OTP);
        }
      }
    } else {
      APFlag = 0;
    }
  }
}
//---------------------------------OLED Display-------------------------------//

void showDisplay() {
  if (otpFlag) {
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
  else if (displayCount==1)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("LPT: ");
    display.setCursor(30, 0);
    showLPT = millis() - LastPT;
    showLPT /= 60000;
    display.println(int(showLPT));
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

    display.setCursor(0, 13);
    display.println("Temperature");
    display.setCursor(78, 13);
    display.println(temp);
    display.setCursor(0, 26);
    display.println("Humidity");
    display.setCursor(78, 26);
    display.println(hum);
    display.setCursor(0, 39);
    display.println("Heat Index");
    display.setCursor(78, 39);
    display.println(HI);
    display.setCursor(0, 52);
    display.println("LDR Value");
    display.setCursor(78, 52);
    display.println(ldr);
    display.display();
  }
  else if(displayCount==2)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("LPT: ");
    display.setCursor(30, 0);
    showLPT = millis() - LastPT;
    showLPT /= 60000;
    display.println(int(showLPT));
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
  }else{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("LPT: ");
    display.setCursor(30, 0);
    showLPT = millis() - LastPT;
    showLPT /= 60000;
    display.println(int(showLPT));
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
    display.println("Air Press");
    display.setCursor(78, 13);
    display.println(airPres);
    display.setCursor(0, 26);
    display.println("Water Press");
    display.setCursor(78, 26);
    display.println(waterPres);
    display.display();    
  }
}

//---------------------------------Reading Data from Sensors-------------------------------//
void readDHT() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  hi = dht.computeHeatIndex(t, h, false);
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  hum = ""; temp = ""; HI = "";
  Humidity = String(h);
  Temperature = String(t);
  heatIndex = String(hi);
  hum = String(h);
  hum += '%';
  temp = String(t);
  temp += 'C';
  HI = String(hi);
  HI += 'C';
  //  Serial.println();
  //  Serial.print("Temp: "); Serial.println(Temperature);
  //  Serial.print("hum: "); Serial.println(Humidity);
  //  Serial.print("heatIndex: "); Serial.println(heatIndex);
}

void readMQ135Am() {
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration
  float ratio; //Define variable for ratio
  //  Serial.println();
  //  Serial.print("sensor_value RAW of MQ135Am: "); Serial.println(analogRead(MQ135Am));
  VRL = ((analogRead(MQ137Am) * 3.3) / 4096);
  VRL = (VRL * 5) / 3.3; //Measure the voltage drop and convert to 0-5V
  Rs = ((5 * RLAm) / VRL) - RLAm; //Use formula to get Rs value
  ratio = Rs / RoAm; // find ratio Rs/Ro

  float ppm = pow(10, ((log10(ratio) - bAm) / mAm)); //use formula to calculate ppm

  //  Serial.print("NH3 (ppm) = "); //Display a ammonia in ppm
  //  Serial.print(ppm);   // set the cursor to column 0, line 1
  //  Serial.print("      Voltage = "); //Display a intro message
  //  Serial.println(VRL);
  Ammo = "";
  Ammo += String(int(ppm)) + "ppm";
  Ammonia = String(ppm);
}
//----------------------------------------MQ7CO------------------------------------//
float MQ135ResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE_CO2*(4096-raw_adc)/raw_adc));
}
 
float MQ135Read(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES_CO2;i++) {
    rs += MQ135ResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL_CO2);
  }
 
  rs = rs/READ_SAMPLE_TIMES_CO2;
 
  return rs;  
}

long MQ135GetGasPercentage(float rs_ro_ratio)
{
     return MQ135GetPercentage(rs_ro_ratio,CO2Curve);
}

long  MQ135GetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
void readMQ135CO2() {
  iPPM_CO2 = 0;
  iPPM_CO2 = MQ135GetGasPercentage(MQ135Read(MQ135CO2)/RoCO2);
  CO2 = "";
  CO2 += String(int(iPPM_CO2)) + "ppm";
  CO2gas = String(iPPM_CO2);
}

//----------------------------------------MQ7CO------------------------------------//
float MQ7ResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE_CO*(4096-raw_adc)/raw_adc));
}
 
float MQ7Read(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES_CO;i++) {
    rs += MQ7ResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL_CO);
  }
 
  rs = rs/READ_SAMPLE_TIMES_CO;
 
  return rs;  
}

long MQ7GetGasPercentage(float rs_ro_ratio)
{
     return MQ7GetPercentage(rs_ro_ratio,COCurve);
}

long  MQ7GetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
void readMQ7() {
  iPPM_CO = 0;
  iPPM_CO = MQ7GetGasPercentage(MQ7Read(MQ7CO)/RoCO);
  CO = "";
  CO += String(int(iPPM_CO)) + "ppm";
  COgas = String(iPPM_CO);
}

void getOxygen() {
  oxygenData = Oxygen.ReadOxygenData(COLLECT_NUMBER);
  O2 = "";
  O2 += String(oxygenData) + '%';
  O2gas = String(oxygenData);

}

void getCO2()
{
  measurement_t m = mhz19_uart->getMeasurement();
  Serial.print("co2: ");
  Serial.println(m.co2_ppm);
}

void getpWater() {
  vWater = analogRead(waterPin) * 3.3 / 4096;
  vWater /= 3.3 * 5; //scaling to 5vdc
  pWater = (vWater - OffSet) * 400;
  waterP=String(pWater);
  waterPres="";
  waterPres=String(pWater);
  waterPres+="Pa";
}

void LDR()
{
  int photocellReading = 0;
  for (int i = 0; i < 10; i++) {
    delay(1);
    photocellReading = photocellReading + analogRead(LDRpin);
  }
  photocellReading = photocellReading / 10;
  if (photocellReading <=  2700)
  {
    ldr = "Light";
  }
  else
  {
    ldr = "Dark";
  }
  //  Serial.println();
  //  Serial.print("photocellReading: "); Serial.println(photocellReading);
  //  Serial.print("ldr: "); Serial.println(ldr);
}

void readPressure(){
  String pressure = String(bmp.readPressure());
  airPres=pressure;
  airPres="";
  airPres=pressure;
  airPres+="Pa"; 
}

void combineSensorData() {
  readDHT();
  readMQ135Am();
  readMQ135CO2();
  readMQ7();
  LDR();
  readPressure();
  getpWater();
}

//---------------------------------Getting GSM Signal Strength-------------------------------//
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

//---------------------------------Accesspoint Client-----------------------------------//
void AP_mode() {
  WiFiClient client = server.available(); // Listen for incoming clients
  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); //storing incoming byte in char
        currentLine += c; //char to String
        if (c == '*') {   //Matching termination byte
          stopFlag = 0;   //Will be 1 when client didnt send termination byte
          client.stop();   //closing connected client
          Serial.println("Client disconnected.");
          Serial.println("");
          break;  //breaking loop
        }
      }
    }
    if (stopFlag) { //checking if termination byte is not sent and client is no longer available
      client.stop(); //closing connected client
      Serial.println("Client disconnected.");
      Serial.println("");
    }
    stopFlag = 1; //setting check high for next time
    configParam = currentLine;  //store String in global String
    if (configParam != "") { //checking if client send nothing
      parseDataFunct(); //if client send something then call parse Data Function
    }
  }
  client.stop();
}

//---------------------------------Station Configuration-------------------------------//
void STA_config() {
  tryAgain=0;
  Serial.print("connecting to...");
  Serial.println(ssid_STA);

  WiFi.begin(ssid_STA, password_STA); //Esp connects to WIFI router

  for (int tr = 0; tr <= 50; tr++)    //Checking if router has
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("...");
      delay(100);
      simFlag = 1;
      dataGetTime = 20000;
    }
    else
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      WiFiConnectFlag = 1;
      dataGetTime = 5000;
      simFlag = 0;
      break;
    }
  }
  Serial.println();
  if (simFlag) {
    setupSIM900();
    WiFiConnectFlag = 0;
  }
}

//---------------------------Parsing Data Received from Client-----------------------------//
void parseDataFunct() {
  if (configParam[0] == '$') {
    int posHeader = configParam.indexOf('$');
    int posTail = configParam.indexOf('*');
    if (posHeader == 0)
    {
      int posComma1 = configParam.indexOf(',');
      ssid = configParam.substring((posComma1), posHeader + 1);
      password = configParam.substring((posTail), posComma1 + 1);

      ssid_STA = ssid.c_str();
      password_STA = password.c_str(); //string to char array
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
    Serial.print("configParam: "); Serial.println(configParam);
    int posHeader = configParam.indexOf('@');
    int posTail = configParam.indexOf('*');
    if (posHeader == 0)
    {
      sendmode = "AP";
      int posComma1 = configParam.indexOf(',');
      int posComma2 = configParam.indexOf(',' , posComma1 + 1);
      int posComma3 = configParam.indexOf(',' , posComma2 + 1);

      output1 = configParam.substring((posComma1), posHeader + 1);
      output2 = configParam.substring((posComma2), posComma1 + 1);
      output3 = configParam.substring((posComma3), posComma2 + 1);
      output4 = configParam.substring((posTail), posComma3 + 1);
      Serial.print("output1: "); Serial.println(output1);
      Serial.print("output2: "); Serial.println(output2);
      Serial.print("output3: "); Serial.println(output3);
      Serial.print("output4: "); Serial.println(output4);

      configParam = "";
      setOutput();
      APFlag = 1;
      getData();

    }
  }
}

//---------------------------------Setting Relay Outputs-------------------------------//
void setOutput() {
  if (output1 == "1") {
    fans = "true";
    digitalWrite(OutLoad1, HIGH);
    Serial.println("Output1 High");
  } else {
    fans = "false";
    digitalWrite(OutLoad1, LOW);
    Serial.println("Output1 low");
  }
  if (output2 == "1") {
    ligths = "true";
    digitalWrite(OutLoad2, HIGH);
    Serial.println("Output2 High");
  } else {
    ligths = "false";
    digitalWrite(OutLoad2, LOW);
    Serial.println("Output2 low");
  }
  if (output3 == "1") {
    foggers = "true";
    digitalWrite(OutLoad3, HIGH);
    Serial.println("Output3 High");
  } else {
    foggers = "false";
    digitalWrite(OutLoad3, LOW);
    Serial.println("Output3 low");
  }
  if (output4 == "1") {
    splinkers = "true";
    digitalWrite(OutLoad4, HIGH);
    Serial.println("Output4 High");
  } else {
    splinkers = "false";
    digitalWrite(OutLoad4, LOW);
    Serial.println("Output4 low");
  }
}
