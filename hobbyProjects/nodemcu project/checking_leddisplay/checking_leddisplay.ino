void setup() {
  Serial.begin(115200);
  Serial.println(); Serial.println(); Serial.println();
  String price="10.53";
  int dot=price.indexOf(".");
  int zero=0;
  if(dot != -1){
    if(dot==1 && price[0]=='0'){
     // lc.setDigit(0,2,zero,true);
     Serial.println("price[0]==0");
    }
    // lc.setChar(0,2,'.',true);
    Serial.println("ok");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
