#include <Wire.h>
#include <ZumoShield.h>

ZumoReflectanceSensorArray linesensors(QTR_NO_EMITTER_PIN);

void setup() {
  Serial.begin(9600);
}

unsigned int sensor_vals[6];
void loop() {
  linesensors.read(sensor_vals);
  Serial.print(sensor_vals[0]);
  Serial.print(" ");
  Serial.print(sensor_vals[1]);
  Serial.print(" ");
  Serial.print(sensor_vals[2]);
  Serial.print(" ");
  Serial.print(sensor_vals[3]);
  Serial.print(" ");
  Serial.print(sensor_vals[4]);
  Serial.print(" ");
  Serial.println(sensor_vals[5]);
  delay(50);
}
