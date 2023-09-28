String parseData;
String Key[7], Value[7], dataParse[7],Data;
void setup() {
  Serial.begin(115200);
  delay(1);
  parsingJSON();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void parsingJSON() {
  //parseData = "{accessToken:\"14a5627aab2b0f6e37bf853e788f6de0919f7c42\",refreshToken:\"3d0071858d489407e07f6a021324755936758408\",expiresin:\"1567674867542\"}";
 
  parseData = "{success:true,fans:false,lights:false}";//,foggers:false,splinkers:false,otp:false,optDisplay:none,deviceConfiguration:false}";

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
