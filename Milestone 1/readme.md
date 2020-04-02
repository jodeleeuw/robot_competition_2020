# Milestone 1

> Create your own code that moves the Zumo at least 4 feet.

## Installation

I started off by installing the `ZumoShield` Arduino library. In the Arduino IDE, select `Tools > Manage Libraries...` and search for Zumo. 

Once installed, I added the `ZumoShield` library to my project code through the `Sketch > Include Library` menu.

## Motor Control

The Zumo library provides the function `setLeftSpeed()` and `setRightSpeed()` to control the motors. The valid range for speed is -400 to 400. I began by setting the speed to 100 for each motor, waiting for 4 seconds, and then setting the speed to 0. After setting the speed to 0 the code enters an infinite loop to pause idenfinitely.

```c++
#include <ZumoShield.h>

ZumoMotors motors;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  motors.setLeftSpeed(100);
  motors.setRightSpeed(100);
  delay(4000);
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  while(true){
   // do nothing. forever. 
  }
}
```

## Testing

As this video shows, 4 second of travel at a speed of 100 was not fast enough to travel 4ft. So I changed the speed to 300. This was plenty fast.

[![YouTube Video](https://img.youtube.com/vi/q5z4fuUra2Q/0.jpg)](https://www.youtube.com/embed/q5z4fuUra2Q)

**Milestone 1: complete!**

