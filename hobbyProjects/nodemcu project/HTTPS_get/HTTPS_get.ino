#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "Chili Garlic FF";
const char* password = "Carqy341";

const String API_KEY = "8590a838-cf37-4695-b361-f0e921154728"; //get your api_key from coinmarketcap
const char* SYMBOL = "XRP";
const char* CONVERT_TO = "USD";
const char fingerprint[] PROGMEM = "335b4fc7b2e210256ed6f2f900744ea21f436195";

String  PostStr="https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?CMC_PRO_API_KEY=" + API_KEY + "&symbol=" + SYMBOL + "&convert=" + CONVERT_TO;
String payload;
int lastP;

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
  Send_Data();

}

void loop()
{

  delay(5000);
}

void Send_Data()
{
WiFiClientSecure client;
HTTPClient http;
const char *Host=PostStr.c_str();
client.connect(Host, 443);

if (client.verify(fingerprint, Host)) 
{  
    http.begin(client, Host);

    String payload;
    if (http.GET() == HTTP_CODE_OK)    
        payload = http.getString();
        Serial.print("payload: "); Serial.println(payload); 
        getPrice();   
}
else 
{
  Serial.println("certificate doesn't match");
}
}

void parsing(){
  DynamicJsonDocument jsonDocument(2024);
  DeserializationError error = deserializeJson(jsonDocument, payload);
  if (error) {
    Serial.println("Deserialization failed");
     
     switch (error.code()) {
      case DeserializationError::Ok:
          Serial.print(F("Deserialization succeeded"));
          break;
      case DeserializationError::InvalidInput:
          Serial.print(F("Invalid input!"));
          break;
      case DeserializationError::NoMemory:
          Serial.print(F("Not enough memory"));
          Serial.println("doc capacity: ");
          Serial.print(jsonDocument.capacity());
          break;
      default:
          Serial.print(F("Deserialization failed"));
          break;
    }
     
    delay(1);
    return;
  }
  JsonObject root = jsonDocument.as<JsonObject>();

  // check API status
  JsonObject status = root["status"];
  int statusErrorCode = status["error_code"];
  if (statusErrorCode != 0) {
    String statusErrorMessage = status["error_message"];
    Serial.print("Error: ");
    Serial.println(statusErrorMessage);
    delay(1);
  }

  JsonObject coin = root["data"][SYMBOL];
  String name = coin["name"];
  String lastUpdated = coin["last_updated"];
  Serial.println("Name: " + name);
  Serial.println("Last updated: " + lastUpdated);

  JsonObject quote = coin["quote"][CONVERT_TO];
  float price = quote["price"];
  Serial.print("Price: $");
  Serial.println(price, 2);
}

void getPrice()
{
  const char *response=payload.c_str();
  const char *Fprice = "price";
  if(StrContains(response,Fprice)){
  int Lprice = payload.indexOf(',',lastP+2);
  String price=payload.substring(lastP+2,Lprice);
  Serial.print("price: "); Serial.println(price);
  }
}

char StrContains(const char *str, const char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
                lastP=sfind[found];
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
