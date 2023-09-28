const int MQ135CO2=13;                               
int RL_VALUE_CO2=1;                                     
                     
                                                    
 
int READ_SAMPLE_INTERVAL_CO2=50;                      
int READ_SAMPLE_TIMES_CO2=1;                                
 
float           CO2Curve[3]  =  {1,0.11,-0.16};    // -0.21/1.30
//float          CO2Curve[3]  = {1,0.1,-0.36};                                                             
float           RoCO2           =  0.95;                 

void setup()
{ 
  
  Serial.begin(115200);                       
}
 
void loop()
{  
  long iPPM_CO2 = 0;
  iPPM_CO2 = MQ135GetGasPercentage(MQ135Read(MQ135CO2)/RoCO2);
  
   
   Serial.println("Concentration of gas ");
   Serial.print("RAW: "); Serial.println(analogRead(MQ135CO2));
   
   Serial.print("CO2: ");
   Serial.print(iPPM_CO2);
   Serial.println(" ppm");    

   delay(2000);
  
}

float MQ135ResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE_CO2*(4096-raw_adc)/raw_adc));
}
 
float MQ135Read(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES_CO2;i++) {
    rs += MQ135ResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL_CO2);
  }
 
  rs = rs/READ_SAMPLE_TIMES_CO2;
 
  return rs;  
}

long MQ135GetGasPercentage(float rs_ro_ratio)
{
     return MQ135GetPercentage(rs_ro_ratio,CO2Curve);
}

long  MQ135GetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
