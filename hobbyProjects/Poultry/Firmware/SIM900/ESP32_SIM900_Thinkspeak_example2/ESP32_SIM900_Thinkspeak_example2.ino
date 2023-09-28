
void setup() {
  Serial2.begin(115200);  /* Define baud rate for software serial communication */
  Serial.begin(115200); /* Define baud rate for serial communication */
}

void loop() {
  Serial.println("TCP Send :");
  Serial2.println("ATE1"); /* Check Communication */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT"); /* Check Communication */
  delay(1000);
  ShowSerialData(); /* Print response on the serial monitor */
  Serial2.println("AT+CREG?"); /* Network registration status */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT+CGATT?");  /* Attached to or detached from GPRS service */ 
  delay(1000); 
  ShowSerialData();
  Serial2.println("AT+CIPSHUT"); /* Deactivate GPRS PDP content */
  delay(1000);
  ShowSerialData();
  Serial2.println("AT+CIPMUX=0");  /* Single TCP/IP connection mode */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CGATT=1"); /* Attach to GPRS Service */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CSTT=\"\",\"\",\"\""); /* Start task and set APN */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIICR"); /* Bring up wireless connection with GPRS */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIFSR"); /* Get local IP address */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();
  Serial2.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  /* Start up TCP connection */
  delay(2000);
  ShowSerialData();
  Serial2.println("AT+CIPSEND"); /* Send data through TCP connection */
  delay(2000);
  ShowSerialData();
  Serial2.println("GET https://api.thingspeak.com/update?api_key=GDKZYY76S4G4HLQX&field1="+String(random(10))+"&field2="+String(random(10,19))+"&field3="+String(random(20,29))+"&field4="+String(random(30,40))+"\r\n");  /* URL for data to be sent to */
  delay(5000);
  ShowSerialData();
  Serial2.println(char(26));
  delay(5000);
  Serial2.println();
  ShowSerialData();
  
  Serial.print("AT+CIPSHUT\\r\\n");
  Serial2.println("AT+CIPSHUT"); /* Deactivate GPRS PDP content */
  delay(1000);
  ShowSerialData();
}

void ShowSerialData()
{
  while(Serial2.available()!=0)  /* If data is available on serial port */
  Serial.write(char (Serial2.read())); /* Print character received on to the serial monitor */
}
