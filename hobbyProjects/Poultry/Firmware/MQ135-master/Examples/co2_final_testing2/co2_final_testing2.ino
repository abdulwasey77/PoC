#include <DHT.h>

#define DHTTYPE DHT22

#include "MQ135.h"

// MQ135 gas sensor
//
// Datasheet can be found here: https://www.olimex.com/Products/Components/Sensors/SNS-MQ135/resources/SNS-MQ135.pdf
//
// Application
// They are used in air quality control equipments for buildings/offices, are suitable for detecting of NH3, NOx, alcohol, Benzene, smoke, CO2, etc
//
// Original creator of this library: https://github.com/GeorgK/MQ135


DHT dht(2, DHTTYPE);

#define PIN_MQ135 13
MQ135 mq135_sensor = MQ135(PIN_MQ135);

float temperature = 28.0; // assume current temperature. Recommended to measure with DHT22
float humidity = 25.0; // assume current humidity. Recommended to measure with DHT22

void setup() {
  Serial.begin(9600);
}

void loop() {
//  float humidity = dht.readHumidity();
//  float temperature = dht.readTemperature();
//  if (isnan(humidity) || isnan(temperature))
//  {
//    Serial.println("Failed to read from DHT sensor!");
//    return;
//  }
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.print("ppm @ temp/hum: ");
  Serial.print(temperature);
  Serial.print("/");
  Serial.print(humidity);
  Serial.println("%");
  delay(600);
}
