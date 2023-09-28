int gas_sensor = 33; //Sensor pin 
float m = -0.6527; //Slope 
float b = 1.30; //Y-Intercept 
float R0 = 3.50; //Sensor Resistance in fresh air from previous code
  float sensor_volt; //Define variable for sensor voltage 
  float RS_gas; //Define variable for sensor resistance  
  float ratio; //Define variable for ratio
  float sensorValue=0;
 
void setup() {
  Serial.begin(115200); //Baud rate 
 
  pinMode(gas_sensor, INPUT); //Set gas sensor as input 
}
 
void loop() {  
 
  for(int i=0 ; i<20 ;i++){
  sensorValue = sensorValue+analogRead(gas_sensor); //Read analog values of sensor  
  }
  sensorValue=sensorValue/20;
  Serial.print("GAS SENSOR RAW VALUE = ");
  Serial.println(sensorValue);
  sensor_volt = sensorValue*(3.3/4096); //Convert analog values to voltage 
  Serial.print("Sensor value in volts = ");
  Serial.println(sensor_volt);
  RS_gas = ((3.3*6.0)/sensor_volt)-6.0; //Get value of RS in a gas
  Serial.print("Rs value = ");
  Serial.println(RS_gas);
  ratio = RS_gas/R0;  // Get ratio RS_gas/RS_air
  
  Serial.print("Ratio = ");
  Serial.println(ratio);
  double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  Serial.print("Our desired PPM = ");
  Serial.println(ppm);
  double percentage = ppm/10000; //Convert to percentage 
  Serial.print("Value in Percentage = "); //Load screen buffer with percentage value
  Serial.println(percentage); 
  delay(2000);
}
