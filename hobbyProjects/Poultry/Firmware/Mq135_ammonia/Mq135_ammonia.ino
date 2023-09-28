#define RL 20  //The value of resistor RL is 47K
#define m -0.404215 //Enter calculated Slope 
#define b 0.864883 //Enter calculated intercept
#define Ro 1 //Enter found Ro value
#define MQ_sensor A0 //Sensor is connected to A4

void setup() {
  Serial.begin(115200);
}

void loop() {

  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
   
  VRL = analogRead(MQ_sensor)*(3.3/4096); //Measure the voltage drop and convert to 0-5V
  Rs = ((3.3*RL)/VRL)-RL; //Use formula to get Rs value
  ratio = Rs/Ro;  // find ratio Rs/Ro
 
  float ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm

  Serial.print("NH3 (ppm) = "); //Display a ammonia in ppm
  Serial.print(ppm);   // set the cursor to column 0, line 1
  Serial.print("      Voltage = "); //Display a intro message 
  Serial.println(VRL);
  delay(3000);
}
