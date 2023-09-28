#include <HTTPClient.h>
#include <WiFiClientSecure.h>
//
//const char* ssid     = "Chili Garlic";
//const char* password = "Mayonnaise200ml";

const char* ssid     = "I-Tech-Router";
const char* password = "0512303189T";


String PostStr;// = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2"; //get it from client id=00:11:22:33:44:55&w=20.3
String parseData, readstring, accessToken, refreshToken;
String Key[10], Value[10], dataParse[10], Data, OTP, fans, ligths, foggers, splinkers, mode;
char accessFlag = 0, refreshFlag = 0, simFlag = 1;
unsigned long min5 = millis(), sec10 = millis();
int httpCode;
char fail = 1;
String mac;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.setRxBufferSize(1024);
  Serial2.setRxBufferSize(1024);

  WiFi.begin(ssid, password);

  Serial.println("Connection To Wifi Router");
  for (int i = 0; i <= 1; i++) {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("..");
      delay(300);
    } else {
      simFlag = 0;
      Serial.println();
      Serial.println("Connected to Wifi Router");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
  }
 // mac = WiFi.macAddress();
 mac="30:AE:02:33:A4";
  Serial.print("Wifi Mac: ");
  Serial.println(mac);
  if (simFlag) {
    setupSIM900();
  }
  //PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac="+mac;
  // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/dataStoring?accessToken=MPfAaEeZfsyDEqa6mPhdmWvcn9k9R7yH1SztYkcvBmU7xVKNbeZvqUGkdDpfTzL6iekSUt7yWJyH3QkoDrHcP5PYwFDqTuc&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
}
void loop()
{

  getToken();
  if (millis() - sec10 > 30000) {
    sec10 = millis();
    
      Serial.println("getting");
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
    getData();
          Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
  }
  if(millis()-min5>100000){
      min5=millis();
      Serial.println("posting");
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      postData();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
    }
}


void setupSIM900() {
  readstring=sendATcommand("ATE1", 1000, 400);
  Serial.println(readstring);
  readstring=sendATcommand("AT", 1000, 400);
  Serial.println(readstring);
  readstring=sendATcommand("AT+CGATT=1", 1000, 400);
  Serial.println(readstring);
  readstring=sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000, 400);
  Serial.println(readstring);
  readstring=sendATcommand("AT+SAPBR=3,1,\"APN\",\"\"", 2000, 400);
  Serial.println(readstring);
  readstring=sendATcommand("AT+SAPBR=1,1", 2000, 1000);
  Serial.println(readstring);
  readstring=sendATcommand("AT+HTTPINIT", 1000, 400);
  Serial.println(readstring);
}

void postSim900API() {
  parseData = "";
  //String API = "backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=30:AE:02:33:A2";
  readstring = "";
  readstring = sendATcommand(PostStr, 4000, 1000);
  Serial.print("Start PostStr :"); Serial.print(readstring); Serial.println("  End  PostStr");

  readstring = sendATcommand("AT+HTTPACTION=0", 10000, 6000);
  Serial.print("Start Action :"); Serial.print(readstring); Serial.println("  End Action");

  parseData = sendATcommand("AT+HTTPREAD", 10000, 6000);
  Serial.print("parseData: "); Serial.println(parseData);

  //  readstring = sendATcommand("AT+HTTPTERM", 2000, 1000);
  //  Serial.println(readstring);

  //  readstring = sendATcommand("AT+SAPBR=0,1", 2000, 1000);
  //  Serial.println(readstring);

  getSIM900String();
}

void getSIM900String() {
  int sBrket = parseData.indexOf('{');
  int eBrket = parseData.indexOf('}');
  parseData = parseData.substring(eBrket+1, sBrket);
  if (parseData == "") {
    setupSIM900();
    postSim900API();
  }
  Serial.print("After Parsing : "); Serial.println(parseData);
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

void postWifiAPI()
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
        httpCode = https.GET();
        if (httpCode > 0)
        {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            String payload = https.getString();
            Serial.println(payload);
            parseData = payload;
            // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2";
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
  //parseData = "{accessToken:\"14a5627aab2b0f6e37bf853e788f6de0919f7c42\",refreshToken:\"3d0071858d489407e07f6a021324755936758408\",expiresin:\"1567674867542\"}";

  // parseData = "{success:true,fans:false,lights:false}";//,foggers:false,splinkers:false,otp:false,optDisplay:none,deviceConfiguration:false}";

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
    Serial.println("Getting access token");
    if (simFlag) {
      PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac="+mac+"\"";
      postSim900API();
    } else {
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=" + mac;
      postWifiAPI();
    }
    parsingJSON();
    if (Key[0] == "\"accessToken\"") {
      accessFlag = 0;
      accessToken = Value[0];
      Serial.print("accessToken: "); Serial.println(accessToken);
    }
    if (Key[1] == "\"refreshToken\"") {
      refreshToken = Value[1];
      Serial.print("accessToken: "); Serial.println(refreshToken);
    }
  }
  if (refreshFlag) {
    Serial.println("Getting refresh token");
    if (simFlag) {
      PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?grantType=refreshToken&refreshToken=" + refreshToken+"\"";
      postSim900API();
    } else {
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?grantType=refreshToken&refreshToken=" + refreshToken;
      postWifiAPI();
    }
    parsingJSON();
    if (Key[0] == "\"accessToken\"") {
      refreshFlag = 0;
      accessToken = Value[0];
      Serial.print("accessToken: "); Serial.println(accessToken);
    }
    if (Key[1] == "\"refreshToken\"") {
      refreshToken = Value[1];
      Serial.print("accessToken: "); Serial.println(refreshToken);
    }
  }
}

void postData() {
  if (simFlag) {
    PostStr = "";
    //PostStr = "http://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=MPfAaEeZfsSfb93XMnMRicdf6coiGuhBywNoDz79TJvo5zYuY21pSioRvRyAeop73Ri6nCiaHKPT6zm3sLssSvSoWQbGxG3&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=\"01/January/1970 00:00:12\"&mode=\"IT\"";
    PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=12/26/1970%2000:00:12&mode=IT\"";
    postSim900API();
    
  } else {
    PostStr = "";
   // PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
      PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=" + accessToken + "&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=12/26/1970%2000:00:12&mode=IT";
   
    postWifiAPI();
  }
  parsingJSON();
  if (httpCode == 400 || httpCode == 401 || httpCode == 403) {
      httpCode=0;
      accessFlag = 1;
      refreshFlag = 1;
    }
  if (Value[0] == "false") {
    accessFlag = 1;
    refreshFlag = 1;
  }
  if (Value[3] == "true") {
    accessFlag = 1;
  }
  Serial.print("Key[0]: "); Serial.println(Key[0]);
  Serial.print("Value[0]: "); Serial.println(Value[0]);
  if (Value[0] == "true") {
    refreshFlag = 0;
    if (Value[1] == "true") {
      OTP = Value[2];
      Serial.print("OTP: "); Serial.println(OTP);
    }
  }
}

void getData() {
  if (simFlag) {
    PostStr = "";
    //PostStr = "http://backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=MPfAaEeZfsSfb93XMnMRicdf6coiGuhBywNoDz79TJvo5zYuY21pSioRvRyAeop73Ri6nCiaHKPT6zm3sLssSvSoWQbGxG3&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z";
    PostStr = "AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z\"";
    postSim900API();
  } else {
    PostStr = "";
    PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataForApplications?accessToken=" + accessToken + "&fans=true&lights=false&foggers=true&splinkers=false&deviceDateTime=01/January/197000:00:12&mode=IT&dateTime=2020-11-21T15:52:28.510Z";
    postWifiAPI();
  }
  parsingJSON();
  if (httpCode == 400 || httpCode == 401 || httpCode == 403) {
      accessFlag = 1;
      refreshFlag = 1;
    }
  if (Value[0] == "false") {
    accessFlag = 1;
    refreshFlag = 1;
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
    }
  }

}
