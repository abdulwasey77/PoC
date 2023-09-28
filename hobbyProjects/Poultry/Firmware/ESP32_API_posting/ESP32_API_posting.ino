#include <HTTPClient.h>
#include <WiFiClientSecure.h>

//const char* ssid     = "Chili Garlic";
//const char* password = "Mayonnaise200ml";

const char* ssid     = "I-Tech-Router";
const char* password = "0512303189T";
String PostStr;// = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac=30:AE:02:33:A2"; //get it from client id=00:11:22:33:44:55&w=20.3
String parseData;
String Key[7], Value[7], dataParse[7],Data;

char fail = 1;
char start = 0;
String header;
int long t1 = 0;
int long t2 = 0;
int long s1 = 0;
int long s2 = 0;
int c = 0;
String mac;

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
  //PostStr = "https://backoffice.poultryapp.com/backup_api/crons/deviceConfiguration?mac="+mac;
 // PostStr = "https://backoffice.poultryapp.com/backup_api/crons/dataStoring?accessToken=MPfAaEeZfsyDEqa6mPhdmWvcn9k9R7yH1SztYkcvBmU7xVKNbeZvqUGkdDpfTzL6iekSUt7yWJyH3QkoDrHcP5PYwFDqTuc&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321";
 //   PostStr = "https://backoffice.poultryapp.com/backup_api/automate/dataStoring?accessToken=XELwpTTw29sXqu1M73nmLou6NjPd6gTQDE3pW8LLX7EpiAYkcxPGGrcu3woppVmy2CB1FdDhhLNx97ARqBHntLryuGyL9ALk64h&temp=30.12&humi=62&heat=34.12&co2g=21&coga=0&ammo=0&airf=0&ldrl=321&deviceDateTime=\"01/01/1970 00:00:12\"&mode=\"IT\"";
    PostStr = "https://coinmarketcap.com/currencies/xrp/markets/";
     Send_Data();

}

void loop()
{

  delay(5000);
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
