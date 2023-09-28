#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3); // tX , rX of 808

int accelX, accelY, accelZ;
char mpu = 0x68;  
float vectorprevious;
float vector;
float totalvector;
int Steps = 0;
String gpslat;
String gpslong;
String api_key= "your_thinkspeak_api_key";
unsigned long postTime;
int battery;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(3000);
 // pinMode(6,OUTPUT);
  pinMode(A0,INPUT);   
  Serial.begin(9600);
  mySerial.begin(9600);
  checkgps();
  setupMPU();
  postTime=millis();
}
void loop() {
  getSteps();
  if(millis()-postTime>1000*300)
  {
    getbattery();
    send_API();
  }
}

void getbattery()
{
  //voltage divided setup using R1=2K and R2=4.7K
  //At 4.2V will get Vout=2.95 and at 3.6V will get Vout=2.53
  //so if we use 3.3v minipro we will have 10bit ADC
  //which means we will have linear scale of 0-1024 corresponding to 0-3.3V
  //so at 2.95 we will have analog value of 915 and at 2.53 we will have analong value of 785
  //so we will map these values from 0-100
  int batt = analogRead(A0);
  battery = map(batt, 785, 915, 0, 100);
  
}
void getSteps(){
   getAccel();
// Serial.print("accelX: "); Serial.println(accelX);
// Serial.print("accelY: "); Serial.println(accelY);
// Serial.print("accelZ: "); Serial.println(accelZ);
 float x=accelX * accelX;
 float y=accelY * accelY;
 float z=accelZ * accelZ;
// Serial.print("accelX sq: "); Serial.println(x);
// Serial.print("accelY sq: "); Serial.println(y);
// Serial.print("accelZ sq: "); Serial.println(z);
 float sum=x+y+z;
// Serial.print("sum: "); Serial.println(sum);
// Serial.println();
 vector = sqrt(sum);
// Serial.print("vector: "); Serial.println(vector);
 totalvector = vector - vectorprevious;
 Serial.print("totalvector: "); Serial.println(totalvector);
 Serial.println();
if (totalvector > 6){
  Steps++;
}
Serial.print("Steps: "); Serial.println(Steps);
vectorprevious = vector;
delay(200); 
}
void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void getAccel() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = (Wire.read()<<8|Wire.read())/163; //Store first two bytes into accelX
  accelY = (Wire.read()<<8|Wire.read())/163; //Store middle two bytes into accelY
  accelZ = (Wire.read()<<8|Wire.read())/163; //Store last two bytes into accelZ

}

void checkgps(){
  String A;
  int gps=0;
  sendATcommand("AT",100);
  A=sendATcommand("AT",500);
  for(int i=0;i==7;i++){
    if( A.charAt(i)=='O'){
    gps=1;
  }
  }
  
  if (gps) { }
  else{
    turngpson();
  }
}

void gps() { 
    int datacount=0;
    char databuffer[64];
    mySerial.println("AT+CGPSINF=0");
    delay(500);
    
    while((mySerial.available()!=0)&&(datacount<63)){
      databuffer[datacount]=mySerial.read();
      datacount++;
    }
    
    databuffer[datacount]=0;
    Serial.println(databuffer);
    String output;
    output=strtok(databuffer,":");
    int field=0;
    
    while(output != NULL) {
      field++;
      output=strtok(NULL,",");
      Serial.println(output); 
      if(field==2){
        gpslat+=output;
      }
      if(field==3){
        gpslong+=output;
      }
    }  
}
void send_API(){ 
    
//    api_key,gpsLat,gpsLong,Steps,battery
    sendATcommand("AT+HTTPPARA=\"URL\",\"api.thingspeak.com/update\"",3000);
    String senddata;
    senddata+=api_key+"field1"+gpslat+"field2"+gpslong+"field3"+String(Steps)+"field4"+String(battery);
    int leng =senddata.length();  
    sendATcommand("AT+HTTPDATA="+String(leng)+",10000" , 2000);
    sendATcommand(senddata, 2000);
    sendATcommand("AT+HTTPACTION=0" , 8000);
    senddata="";
}

String sendATcommand(String toSend, unsigned long milliseconds) {
    String result;
    mySerial.println(toSend);
    unsigned long startTime = millis();
    while (millis() - startTime < milliseconds) {
      if (mySerial.available()) {
        char c = mySerial.read();
        result += c;
      }
    }
    return result;
}


void turngpson(){
    digitalWrite(6,HIGH);
    digitalWrite(A0,HIGH);
    delay(3000);
    digitalWrite(A0,LOW);
    delay(3000);
    sendATcommand("AT+CGPSPWR=1" , 2000);
    sendATcommand("AT+HTTPINIT" , 2000);
    sendATcommand("AT+HTTPPARA=CID,1" , 2000);
    sendATcommand("AT+SAPBR=3,1,APN,WAP.CINGULAR" , 2000);
    sendATcommand("AT+SAPBR=1,1" , 2000);
    delay(2000);
    digitalWrite(6,LOW);
  }
