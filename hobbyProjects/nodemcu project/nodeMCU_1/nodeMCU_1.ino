#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <LedControl.h>

const char* ssid = "your ssid";           //enter your router ssid here
const char* password = "your password";   //enter your router password here

int GreenLed = D1; //greenled will  connect to pin D1 of node MCU
int RedLed = D2;   //redled will  connect to pin D1 of node MCU

const String API_KEY = "8590a838-cf37-4695-b361-f0e921154728";     //replace your api-key with this if it doesnt work

const char* SYMBOL = "XRP";
const char* CONVERT_TO = "USD";

const long DEFAULT_DELAY = 60 * 1000 * 5; // 5min Wait;

const char* host = "pro-api.coinmarketcap.com";
const int httpsPort = 443;

String payload;
int lastP;
String price;
float lastprice=0.0,thisprice=0.0;
const char fingerprint[] PROGMEM = "335b4fc7b2e210256ed6f2f900744ea21f436195";

WiFiClientSecure client;

int dataPin = D5;    //The pin on the Arduino where data gets shifted out
int csPin = D6;      //The pin for selecting the device when data is to be sent
int clockPin = D7;   //The pin for the clock
int numDevices = 8; //The maximum number of devices that can be controlled

LedControl lc=LedControl(dataPin, clockPin, csPin, numDevices);

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(GreenLed,OUTPUT);
  pinMode(RedLed,OUTPUT);
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // init LedControl 
  Serial.print("index: ");
  Serial.println(lc.getDeviceCount());
    for(int index=0;index<lc.getDeviceCount();index++) {
        lc.shutdown(index,false);
        /* Set the brightness to a medium values */
        lc.setIntensity(index,8);
        /* and clear the display */
        lc.clearDisplay(index);
    }
   printPrice("99999999");


  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
}


void loop() {
  
  client.setFingerprint(fingerprint);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/v1/cryptocurrency/quotes/latest?CMC_PRO_API_KEY=" + API_KEY + "&symbol=" + SYMBOL + "&convert=" + CONVERT_TO;
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  // check HTTP response
  char httpStatus[32] = {0};
  client.readBytesUntil('\r', httpStatus, sizeof(httpStatus));
  if (strcmp(httpStatus, "HTTP/1.1 200 OK") != 0) {
    Serial.print("Unexpected response: ");
    Serial.println(httpStatus);
    delay();
    return;
  }

  // skip HTTP headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
  //  Serial.println(line);
    if (line == "\r") {
      break;
    }
  }

  // skip content length
  if (client.connected()) {
    String line = client.readStringUntil('\n');
   // Serial.println(line);
  }

  // get response
  payload = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
  //  Serial.println(line);
    line.trim();
    if (line != "\r") {
      payload += line;
    }
  }

  client.stop();
  
  Serial.println();
  Serial.print("payload: "); Serial.println(payload);
  Serial.println();
  // parse response
  getPrice();
  showTrend();
  
  delay();
}

void getPrice()
{
  const char *response=payload.c_str();
  const char *Fprice = "price";

  if(StrContains(response,Fprice)){
  int Lprice = payload.indexOf(',',lastP+2);
  price=payload.substring(lastP+3,Lprice);
  Serial.print("price: "); Serial.println(price);
  thisprice=price.toFloat();
  printPrice(price);
  price="";
  }
}

void showTrend(){
  if(thisprice>=lastprice)
  {
    lastprice=thisprice;
    digitalWrite(GreenLed,HIGH);
    digitalWrite(RedLed,LOW);
  }
  else
  {
    lastprice=thisprice;
    digitalWrite(GreenLed,LOW);
    digitalWrite(RedLed,HIGH);
  }
}

unsigned int StrContains(const char *str, const char *sfind)
{
    unsigned int found = 0;
    unsigned int index = 0;
    unsigned int len;

    len = strlen(str);
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                lastP=index;
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}


void printPrice(String price){
  // reset
  for(int index=0;index<lc.getDeviceCount();index++) {
    lc.clearDisplay(index);
  }
  int dot=price.indexOf(".");
  int zero=0;
  int disPrice;
  float Fprice;
  if(dot != -1){
    if(dot==1 && price[0]=='0'){
     lc.setDigit(0,2,zero,true);
    }
    lc.setChar(0,2,'.',true);
    Fprice=price.toFloat()*100;
    disPrice=int(Fprice);
  }
  else
  {
    Fprice=price.toFloat()*100;
    disPrice=int(Fprice);
  }
  int index = floor((8-sizeof(disPrice)) /2);
  while(disPrice > 0){
    lc.setDigit(0, index, disPrice%10, false);
    disPrice = disPrice/10;
    index++;
  }
}
    
  
void delay() {
  delay(DEFAULT_DELAY);
}
