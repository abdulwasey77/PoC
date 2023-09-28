#define RL 20  //The value of resistor RL is 47K
long analog_value;
float VRL;
float Rs;
float Ro;
void setup() //Runs only once
{
  Serial.begin(115200); //Initialise serial COM for displaying the value
}

void loop() {

//  for(int test_cycle = 1 ; test_cycle <= 500 ; test_cycle++) //Read the analog output of the sensor for 200 times
//  {
//    analog_value = analog_value + analogRead(A0); //add the values for 200
//  }
for(int i=0; i<=1000; i++){
  analog_value=analog_value+analogRead(13);
}
  analog_value = analog_value/1000; //Take average
  Serial.print("analog_value = ");
  Serial.println(analog_value);
  VRL = float(analog_value*(3.3/4096.0));//Convert analog value to voltage
  Serial.print("VRL = ");
  Serial.println(VRL);
  //RS = ((Vc/VRL)-1)*RL is the formulae we obtained from datasheet
  Rs = ((3.3/VRL)-1) * RL;
  Serial.print("Rs = ");
  Serial.println(Rs);
  //RS/RO is 3.6 as we obtained from graph of datasheet
  Ro = Rs/3.6;
  Serial.print("Ro at fresh air = ");
  Serial.println(Ro); //Display calculated Ro
  delay(1000); //delay of 1sec
}
