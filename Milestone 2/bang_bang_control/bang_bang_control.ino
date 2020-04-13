#include <Wire.h>
#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray linesensors(QTR_NO_EMITTER_PIN);

void setup() {
  int i;
  int spin_direction = 1;
  motors.setSpeeds(80*spin_direction, -80*spin_direction);
  for(i = 0; i<150; i++){
    linesensors.calibrate();
    if(i%50 == 25){ // every 50 loops, starting on loop 25...
      spin_direction = -spin_direction;
      motors.setSpeeds(80*spin_direction, -80*spin_direction);
    }
    delay(20);
  }
  motors.setSpeeds(0,0);
  delay(500);
}

unsigned int sensor_vals[6];
void loop() {
  int line_position = linesensors.readLine(sensor_vals);
  int error = line_position - 2500;
  if(error > 250){
    motors.setSpeeds(200,100);
  } else if(error < -250){
    motors.setSpeeds(100,200);
  } else {
    motors.setSpeeds(150,150);
  }
}
