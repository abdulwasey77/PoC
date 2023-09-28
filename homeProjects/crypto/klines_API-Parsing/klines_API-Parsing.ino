String parseData;
String Key[7], Value[7], dataParse[7],Data;
void setup() {
  Serial.begin(115200);
  delay(1);
  parsingJSON();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void parsingJSON() {
  parseData ="[[1616850900000,\"0.30890000\",\"0.31500000\",\"0.30890000\",\"0.31410000\",\"333809.54000000\",1616851799999,\"104293.89489900\",513,\"198701.68000000\",\"62087.49119600\",\"0\"],[1616851800000,\"0.31370000\",\"0.31840000\",\"0.31360000\",\"0.31670000\",\"492292.30000000\",1616852699999,\"155688.20724200\",646,\"290543.08000000\",\"91825.54944300\",\"0\"],[1616852700000,\"0.31670000\",\"0.32000000\",\"0.31520000\",\"0.31950000\",\"272447.99000000\",1616853599999,\"86714.75343300\",368,\"200077.40000000\",\"63768.25464400\",\"0\"],[1616853600000,\"0.31950000\",\"0.32000000\",\"0.31600000\",\"0.31940000\",\"246875.89000000\",1616854499999,\"78640.90910500\",439,\"133253.32000000\",\"42448.48400500\",\"0\"],[1616854500000,\"0.31940000\",\"0.32000000\",\"0.31810000\",\"0.31960000\",\"137596.47000000\",1616855399999,\"43933.88247900\",224,\"103870.05000000\",\"33184.52578500\",\"0\"]]";
 Serial.print("parseData : "); Serial.println(parseData);
  int limit = 5;
  String kline[limit],klineO[limit],klineC[limit];
  int  index=1;
  for(int i=0; i<limit; i++)
  {
    int brE = parseData.indexOf(']',index);
   // Serial.print("brE : "); Serial.println(brE);
    kline[i] = parseData.substring(brE+1,index);
    index=brE+2;
    int index2=0;
    for(int j=0; j<5; j++){
      int comma= kline[i].indexOf(',',index2);
      if(j==1){
        klineO[i]=kline[i].substring(comma-1,index2+1);
      }
      if(j==4){
        klineC[i]=kline[i].substring(comma-1,index2+1);
      }
      index2=comma+1;
    }
    Serial.print("kline["); Serial.print(i); Serial.print("] : "); Serial.println(kline[i]);
    Serial.print("klineO["); Serial.print(i); Serial.print("] : "); Serial.println(klineO[i]);
    Serial.print("klineC["); Serial.print(i); Serial.print("] : "); Serial.println(klineC[i]);
    
  }

}
