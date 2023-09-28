

// Configure software serial port
//HardwareSerial Serial2(1);
#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200);
  // Arduino communicates with Serial2 GSM shield at a baud rate of 19200
  // Make sure that corresponds to the baud rate of your module
  Serial2.begin(115200);
  // Give time to your GSM shield log on to network
 // delay(20000);   
//  Serial.println("Sending SMS");
  // Send the SMS
  
}

void loop() { 
//  sendSMS();
//  delay(20000);
//  Serial.println("Sending SMS");
 //      listen for communication from the ESP8266 and then write it to the serial monitor
    if ( Serial2.available() )   {  Serial.write(Serial2.read());  }
 
    // listen for user input and send it to the ESP8266
    if ( Serial.available() )       {  Serial2.write( Serial.read() );  }
}
void sendSMS() {
  // AT command to set Serial2 to SMS mode
  Serial2.print("AT+CMGF=1\r"); 
  delay(100);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  Serial2.println("AT + CMGS = \"+923455897543\""); 
  delay(100);
  
  // REPLACE WITH YOUR OWN SMS MESSAGE CONTENT
  Serial2.println("Message example from Arduino Uno."); 
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  Serial2.write(0x1A);
  delay(100);
  Serial2.println();
  // Give module time to send SMS
  delay(5000); 
}
