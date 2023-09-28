#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid     = "Chili Garlic";
const char* password = "Mayonnaise200ml";

//const char* ssid     = "I-Tech-Router";
//const char* password = "0512303189T";
String PostStr;
String parseData;

String mac;
String Key[20], Value[20], dataParse[20],Data;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid,password);

  Serial.print("Connection To Wifi Router");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print("..");
    delay(300); 
  }
  Serial.println();
  Serial.println("Connected to Wifi Router");
  mac = WiFi.macAddress();
  Serial.print("Wifi Mac: ");
  Serial.println(mac);
  PostStr = "https://api.binance.com/api/v3/ticker/24hr?symbol=LUNAUSDT";
  Send_Data();
  parsingJSON();
}

void loop()
{

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
      if (https.begin(*client, PostStr))
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
      Data=dataParse[i];
      int collon = Data.indexOf(':');
      int lenData = Data.length();
      Key[i] = Data.substring(collon, 0);
      Value[i] = Data.substring(lenData, collon + 1);
      nameKey = "Key[" + String(i) + "] :  ";
      nameValue = "       Value[" + String(i) + "] :  ";
      Serial.print(nameKey); Serial.print(Key[i]);
      Serial.print(nameValue); Serial.println(Value[i]);
      i++;
    }
    if (thiscomma >= 0)
    {
      dataParse[i] = parseData.substring(thiscomma, lastcomma + 1);
      Data=dataParse[i];
      int collon = Data.indexOf(':');
      int lenData = Data.length();
      Key[i] = Data.substring(collon, 0);
      Value[i] = Data.substring(lenData, collon + 1);
    }
    else
    {
      dataParse[i] = parseData.substring(len - 1, lastcomma + 1);
      Data=dataParse[i];
      int collon = Data.indexOf(':');
      int lenData = Data.length();
      Key[i] = Data.substring(collon, 0);
      Value[i] = Data.substring(lenData, collon + 1);
    }
    nameKey = "Key[" + String(i) + "] :  ";
    nameValue = "       Value[" + String(i) + "] :  ";
    Serial.print(nameKey); Serial.print(Key[i]);
    Serial.print(nameValue); Serial.println(Value[i]);
    i++;
  }
}
