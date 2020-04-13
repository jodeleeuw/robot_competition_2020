#include <ZumoShield.h>

ZumoMotors motors;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  motors.setSpeeds(300,300);
  delay(4000);
  motors.setSpeeds(0,0);
  while(true){
    // do nothing
  }

}
