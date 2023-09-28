String readstring;
void setup() {
  Serial2.begin(115200);  /* Define baud rate for software serial communication */
  Serial.begin(115200); /* Define baud rate for serial communication */
}

void loop() {
  Serial2.println("ATE1"); 
  delay(500);
  ShowSerialData();
  Serial2.println("AT"); /* Check Communication */
  delay(500);
  ShowSerialData();
  Serial2.println("AT+CGATT=1"); 
  delay(500);
  ShowSerialData();
  Serial2.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  /* Connection type GPRS */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT+SAPBR=3,1,\"APN\",\"\"");  /* APN of the provider */
  delay(1000);
  ShowSerialData();
//  readstring="";
//  long timecheck=millis();
//  while(readstring!="OK"){
//  Serial2.println("AT+SAPBR=1,1");
//  delay(8000);
//  ShowSerialData();
//  Serial.print("readstring : "); Serial.println(readstring);
//  if(millis()-timecheck>=30000)
//  break;
//  }
  Serial2.println("AT+SAPBR=1,1");
  delay(8000);
  ShowSerialData();
  
  Serial2.println("AT+HTTPINIT");  /* Initialize HTTP service */
  delay(500); 
  ShowSerialData();
  Serial2.println("AT+HTTPPARA=\"URL\",\"api.thingspeak.com/update\"");  /* Set parameters for HTTP session */
  delay(3000);
  ShowSerialData();
  String senddata = "api_key=GDKZYY76S4G4HLQX&field1="+String(random(10))+"&field2="+String(random(10,19))+"&field3="+String(random(20,29))+"&field4="+String(random(30,40));
  int leng =senddata.length();
  Serial2.println("AT+HTTPDATA="+String(leng)+",10000"); /* POST data of size 33 Bytes with maximum latency time of 10seconds for inputting the data*/ 
  delay(2000);
  ShowSerialData();
  Serial2.println(senddata);
  delay(2000);
  ShowSerialData();
//  readstring="";
//  long timecheck1=millis();
//  while(readstring!="OK"){
//  Serial2.println("AT+HTTPACTION=1");  /* Start POST session */
//  delay(8000);
//  ShowSerialData();
//  Serial.print("readstring : "); Serial.println(readstring);
//  if(millis()-timecheck1>=30000)
//  break;
//  }
  Serial2.println("AT+HTTPACTION=1");  /* Start POST session */
  delay(8000);
  ShowSerialData();
  
  Serial2.println("AT+HTTPTERM");  /* Terminate HTTP service */
  delay(500);
  ShowSerialData();

}

void ShowSerialData()
{
  readstring="";
  while(Serial2.available()!=0){  /* If data is available on serial port */
   Serial.write(char (Serial2.read()));
   //char c = Serial.read();
   // Serial.print(c);
   //readstring+=c;
  }
  //Serial.print("readstring : "); Serial.println(readstring);
  delay(500);
}
