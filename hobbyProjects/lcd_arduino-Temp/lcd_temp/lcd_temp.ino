#include <LiquidCrystal.h>       

#define nextButton 2
#define backButton 3
#define ledPin     8            
#define tempPin    A0              

int nextFlag=0,backFlag=0;
int tempCount=273,temp;
float t;
unsigned long NBtime = 0, BBtime = 0;
unsigned long disTemp = millis(),serTemp = millis();
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  Serial.begin(9600);
  pinMode(nextButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(nextButton), nxtBtn, RISING);
  attachInterrupt(digitalPinToInterrupt(backButton), bckBtn, RISING);
  nextFlag=0,backFlag=0;
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  
}

void loop() {
  tempGet();
  checkButtonpress();
  if(temp>=tempCount){
    digitalWrite(ledPin,HIGH);
  }else{
    digitalWrite(ledPin,LOW);
  }
  if (millis() - disTemp > 500) {
    disTemp = millis();
    showDisplay();
  }
  if (millis() - serTemp > 3000) {
    serTemp = millis();
    showSerial();
  }
}
void tempGet(){
  int reading = analogRead(tempPin);  
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  float t = (voltage - 0.5) * 100 ; 
  temp=int(t)+273;
}

void showDisplay(){
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Temp: ");
   lcd.print(temp);
   lcd.print("K");
   lcd.setCursor(0, 1);
   lcd.print("MAX:  ");
   lcd.print(tempCount);
   lcd.print("K");   
}

void showSerial(){
  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("MAX:  "); Serial.println(tempCount);
}
//----------------------buttons------------------//

void nxtBtn() {
  nextFlag = 1; //set Flag on button
  NBtime = millis(); //getting when button is pressed
}

void bckBtn() {
  backFlag = 1; //set Flag on button
  BBtime = millis(); //getting when button is pressed
}

void checkButtonpress() {
  if (nextFlag && (millis() - NBtime > 250)) {
    nextFlag = 0;
    tempCount++;
   // Serial.print("tempCount: "); Serial.println(tempCount);
  }
  if (backFlag && (millis() - BBtime > 250)) {
    backFlag = 0;
    tempCount--;
  //  Serial.print("tempCount: "); Serial.println(tempCount);
  }
}
