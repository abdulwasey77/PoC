#include "MHZ19.h"
const int rx_pin = 13; //Serial rx pin no
const int tx_pin = 15; //Serial tx pin no

MHZ19 *mhz19_uart = new MHZ19(rx_pin,tx_pin);

void setup() {
    Serial.begin(115200);
    mhz19_uart->begin(rx_pin, tx_pin);

}

void loop() {
  getCO2();
  delay(5000);

}


void getCO2()
{
  measurement_t m = mhz19_uart->getMeasurement();
  Serial.print("co2: ");
  Serial.println(m.co2_ppm);
}
