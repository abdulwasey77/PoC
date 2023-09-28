const int MQ7CO=13;                               
int RL_VALUE_CO=5;                                     
                                                                 
int READ_SAMPLE_INTERVAL_CO=50;                      
int READ_SAMPLE_TIMES_CO=1;                                
 
float           COCurve[3]  =  {1.77,0.20,-1.36};
                                                                                                    
float           RoCO           =  2.4;                 

void setup()
{ 
  
  Serial.begin(9600);                       
}
 
void loop()
{  
  long iPPM_CO = 0;
  iPPM_CO = MQ7GetGasPercentage(MQ7Read(MQ7CO)/RoCO);
  
   
   Serial.println("Concentration of gas ");
   Serial.print("RAW: "); Serial.println(analogRead(MQ7CO));
   
   Serial.print("CO: ");
   Serial.print(iPPM_CO);
   Serial.println(" ppm");    

   delay(2000);
  
}

float MQ7ResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE_CO*(4096-raw_adc)/raw_adc));
}
 
float MQ7Read(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES_CO;i++) {
    rs += MQ7ResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL_CO);
  }
 
  rs = rs/READ_SAMPLE_TIMES_CO;
 
  return rs;  
}

long MQ7GetGasPercentage(float rs_ro_ratio)
{
     return MQ7GetPercentage(rs_ro_ratio,COCurve);
}

long  MQ7GetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
