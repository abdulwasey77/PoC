#include <Wire.h>
int accelX, accelY, accelZ;
char mpu = 0x68;  
float vectorprevious;
float vector;
float totalvector;
int Steps = 0;
void setup() {
  Wire.begin();
  Serial.begin(115200);
  setupMPU();
}
void loop() {
 getAccel();
 Serial.print("accelX: "); Serial.println(accelX);
 Serial.print("accelY: "); Serial.println(accelY);
 Serial.print("accelZ: "); Serial.println(accelZ);
 float x=accelX * accelX;
 float y=accelY * accelY;
 float z=accelZ * accelZ;
 Serial.print("accelX sq: "); Serial.println(x);
 Serial.print("accelY sq: "); Serial.println(y);
 Serial.print("accelZ sq: "); Serial.println(z);
 float sum=x+y+z;
 Serial.print("sum: "); Serial.println(sum);
 Serial.println();
 vector = sqrt(sum);
 Serial.print("vector: "); Serial.println(vector);
 totalvector = vector - vectorprevious;
 Serial.print("totalvector: "); Serial.println(totalvector);
 Serial.println();
if (totalvector > 6){
  Steps++;
}
Serial.print("Steps: "); Serial.println(Steps);
vectorprevious = vector;
delay(200);
//OzOled.clearDisplay(); 
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
