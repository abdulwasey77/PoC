#include <Keypad.h>
#include <LiquidCrystal.h>      
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '.', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int count=0;
char num[10];
String result;

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter : ");
}
  
void loop(){
  char customKey = customKeypad.getKey();
  if(customKey)
  {
   if(customKey=='A' || customKey=='B' || customKey=='C' || customKey=='D' || customKey=='*'){}
   
   else
   {
    num[count]=customKey;
    Serial.print(num[count]);
    lcd.setCursor(8+count, 0);
    lcd.print(num[count]);
    count++;
    lcd.setCursor(0, 1);
    lcd.print("                ");
   }
  }
  if (customKey=='*')
  {
   result="";   
   result=String(num);
   Serial.print("Result String: "); Serial.println(result); 
   memset(num, 0, sizeof num);
    if(count<=6)
    {
        long value = atol(result.c_str());
        Serial.print("Result int : "); Serial.println(value);
        value+=10;
        Serial.println();
        Serial.print("Result : "); Serial.println(value);
        lcd.setCursor(0, 1);
        lcd.print("Result : ");
        lcd.print(value);
    }
      else
    {
        Serial.println();
        Serial.println("Error Wrong Input");
    }
    count=0;
  }
}
