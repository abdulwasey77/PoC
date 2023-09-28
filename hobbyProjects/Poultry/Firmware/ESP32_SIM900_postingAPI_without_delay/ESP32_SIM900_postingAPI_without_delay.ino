char postAPI=1,Action=0,Resp=0,postapiFlag=0,actionFlag=0,readresFlag=0;
unsigned long postmillis;
String simPrcnt;

String readstring;
void setup() {
  Serial2.begin(115200);  /* Define baud rate for software serial communication */
  Serial.begin(115200); /* Define baud rate for serial communication */
  Serial.setRxBufferSize(1024);
  Serial2.setRxBufferSize(1024);
  setupSIM900();
}

void loop() {

Sim900post();
}

void setupSIM900() {
    readstring = sendATcommand("ATE1", 1000, 400);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT", 1000, 400);
    Serial.println(readstring);

//    readstring = sendATcommand("AT+CMGF=1\r", 1000, 400);
//    Serial.println(readstring);
    
    readstring = sendATcommand("AT+CGATT=1", 1000, 400);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000, 400);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+SAPBR=3,1,\"APN\",\"\"", 2000, 400);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+SAPBR=1,1", 2000, 1000);
    Serial.println(readstring);
  
    readstring = sendATcommand("AT+HTTPINIT", 1000, 400);
    Serial.println(readstring);

}
void Sim900post(){
  //String API = "backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=30:AE:02:33:A2";
  
  
  if(postAPI){
    postAPI=0;
    Serial2.println("AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=30:AE:02:33:A2\"");
    postmillis=millis();
    Serial.println("posting API");
    postapiFlag=1;
  }
  if(postapiFlag==1 && millis()-postmillis>5000){
     readstring=Responce();
     postapiFlag=0;
    Serial.println(readstring);
     Action=1;
  }

    if(Action){
    Action=0;
    Serial2.println("AT+HTTPACTION=0");
    postmillis=millis();
    Serial.println("Action HTTP");
    actionFlag=1;
  }
  if(actionFlag==1 && millis()-postmillis>10000){
     readstring=Responce();
     actionFlag=0;
    Serial.println(readstring);
     Resp=1;
  }

    if(Resp){
    Resp=0;
    Serial2.println("AT+HTTPREAD");
    postmillis=millis();
    Serial.println("Reading Responce");
    readresFlag=1;
  }
  if(readresFlag==1 && millis()-postmillis>10000){
     readstring=Responce();
     readresFlag=0;
     Serial.print("Reading: ");  Serial.println(readstring);
     postAPI=1;
  }
//  readstring = sendATcommand("AT+HTTPPARA=\"URL\",\"backoffice.poultryapp.com/backup_api/automate/deviceConfiguration?mac=30:AE:02:33:A2\"", 4000, 1000);
//  Serial.println(readstring);
//  //
//  readstring = sendATcommand("AT+HTTPACTION=0", 10000, 6000);
//  Serial.println(readstring);
//  //
//  readstring = sendATcommand("AT+HTTPREAD", 10000, 6000);
//  Serial.print("readstring : "); Serial.print(readstring); Serial.println("   readstring end ") ;

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

String Responce(){
  String result;
    while(Serial2.available()) {
      char c = Serial2.read();
      result += c;
    }
  return result;
}

void simSignal(){
   simPrcnt = sendATcommand("AT+CSQ\r", 1000, 400);
  Serial.println(simPrcnt);
    int comma=simPrcnt.indexOf(',');
    int collon=simPrcnt.indexOf(':');
    simPrcnt=simPrcnt.substring(comma,collon+2);
    Serial.print("afterParsing:"); Serial.println(simPrcnt);
    int simSgnl=simPrcnt.toInt();
    if(simSgnl>=0){
      if(simSgnl<32){
        simSgnl=(simSgnl*100)/31;
        simPrcnt=String(simSgnl);
      }else{
        simPrcnt='0';
      }
    }else{
      simPrcnt='0';
    }
    Serial.print("SimPrcnt:"); Serial.println(simPrcnt);
}
