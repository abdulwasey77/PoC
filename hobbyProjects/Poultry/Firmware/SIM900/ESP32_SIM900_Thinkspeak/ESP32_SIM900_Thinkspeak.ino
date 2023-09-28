
void setup()
{
  Serial2.begin(115200);               
  Serial.begin(115200);    
}
 
void loop()
{     
  delay(2);            
   senddata(); 
   
  if (Serial2.available())
    Serial.write(Serial2.read());
}
void senddata()
{
  Serial.println("AT : ");
  Serial2.println("AT");
  delay(1000);
  Serial.println("AT+CPIN?");
  Serial2.println("AT+CPIN?"); // pin code verification
  delay(1000);
// REGISTRATION ON THE WEB
Serial.println("AT+CREG?");
  Serial2.print("AT+CREG?");
  delay(1000);
//CONNECTION TO BE MADE GPRS
Serial.println("AT+CGATT?");
  Serial2.println("AT+CGATT?");
  delay(1000);
//CALL CLOSURE
Serial.println("AT+CIPSHUT");
  Serial2.println("AT+CIPSHUT");
  delay(1000);
Serial.print("CONNECTION STATUS : ");
 Serial2.println("AT+CIPSTATUS");
  delay(2000);
// START single IP CONNECTION
Serial.println("AT+CIPMUX ");
  Serial2.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 Serial.print("APN : ");

  Serial2.println("AT+CSTT=\"ZONGWAP\"");
  //IMPORTANT: internet this is APN name of my network. You have to change this because every network has different APN details. For more read SIM900 documentation
  delay(1000);
 
  ShowSerialData();
 Serial.println("GPRS connection status : ");
  Serial2.println("AT+CIICR");
  delay(3000);
 
  ShowSerialData();
 Serial.println("Local IP address: ");
 Serial2.println("AT+CIFSR");
  delay(2000);
 
  ShowSerialData();
 
  Serial2.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  Serial.println("TCP: ");
  Serial2.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  delay(6000);
 
  ShowSerialData();
 Serial.println("send data");
  Serial2.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();

    // paste thingspeak API Write Key and fields amount + string from correct source (+"&field3="+xxXx +"&field4="+xxxx)
    
    String str="GET /update?api_key=GDKZYY76S4G4HLQX&field1="+String(random(10))+"&field2="+String(random(10,19))+"&field3="+String(random(20,29))+"&field4="+String(random(30,40));
    
  Serial2.println(str);
  delay(4000);
  ShowSerialData();

  Serial2.println((char)26); 
  delay(5000);
  Serial2.println();
 
  ShowSerialData();
 
  Serial2.println("AT+CIPSHUT");//closing connection
  delay(100);
  ShowSerialData();
} 
void ShowSerialData()
{
  while(Serial2.available()!=0)
    Serial.write(Serial2.read());
}
