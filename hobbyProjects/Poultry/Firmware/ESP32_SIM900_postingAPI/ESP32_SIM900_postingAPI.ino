String readstring;
void setup() {
  Serial2.begin(115200);  /* Define baud rate for software serial communication */
  Serial.begin(115200); /* Define baud rate for serial communication */
  Serial.setRxBufferSize(1024);
  Serial2.setRxBufferSize(1024);
  setupSIM900();
}

void setupSIM900() {
    readstring = sendATcommand("ATE1", 1000, 400);
    Serial.println(readstring);
    
    readstring = sendATcommand("AT", 1000, 400);
    Serial.println(readstring);
    
//    readstring = sendATcommand("AT+CGATT=1", 1000, 400);
//    Serial.println(readstring);
  
    readstring = sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000, 400);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+SAPBR=3,1,\"APN\",\"\"", 2000, 400);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+SAPBR=1,1", 2000, 1000);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+HTTPINIT", 1000, 400);
    Serial.println(readstring);

}
void loop() {
Sim900post();
}

void Sim900post(){
  //String API = "backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=30:AE:02:33:A2";

  readstring = sendATcommand("AT+HTTPPARA=\"URL\",\"api.binance.com/api/v3/ticker/price?symbol=ADAUSDT\"", 1000, 400);
  Serial.println(readstring);
  //
  readstring = sendATcommand("AT+HTTPACTION=0", 15000, 8000);
  Serial.println(readstring);
  //
  readstring = sendATcommand("AT+HTTPREAD", 1000, 400);
  Serial.print("readstring : "); Serial.print(readstring); Serial.println("   readstring end ") ;

  //  readstring = sendATcommand("AT+HTTPTERM", 2000, 1000);
  //  Serial.println(readstring);

  //  readstring = sendATcommand("AT+SAPBR=0,1", 2000, 1000);
  //  Serial.println(readstring);  
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
