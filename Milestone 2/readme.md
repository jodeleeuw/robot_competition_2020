# Milestone 2

## Tutorial: Using the Zumo Line Sensor Array

The Zumo shield has an array of six short-range IR reflectance sensors under the front of the robot. 

### Initializing the Sensor Array

If you have installed the ZumoShield library (see video from first milestone) then you can go to `Sketch > Include Library`
and select `ZumoShield`.

Create an instance of the `ZumoReflectanceSensorArray` in your sketch like this:

```c++
#include <Wire.h>
#include <ZumoShield.h>

ZumoReflectanceSensorArray linesensors(QTR_NO_EMITTER_PIN);

void setup(){

}

void loop(){

}
```

Specifying `QTR_NO_EMITTER_PIN` sets the option that we do not want to actively turn the IR emitters on and off.
Instead they will be on by default the entire time.

### Reading data 

The data from the line sensor array can be accessed in a few different ways using the `ZumoShield` library.

#### Option 1: Line Location

The first option is to use the `readLine()` function from the library. This function aggregates the data across
the six sensors and returns a value from `0-5000` indicating the estimated location of the line. A value of `0`
indicates that the line is all the way to the left of the robot, a value of `5000` indicates that the line is 
all the way to the right of the robot, and a value of `2500` indicate that the line is centered under the robot.

In order to use the `readLine()` function you must first calibrate the line sensors. This is accomplished by
calling the `calibrate()` method and exposing each of the six sensors to the darkest and lightest values in
the environment. One way to accomplish this is to have the robot oscillate left-and-right for a few seconds
while repeatedly calling the calibrate function. 

```c++
void setup() {
  int i;
  int spin_direction = 1;
  motors.setSpeeds(80*spin_direction, -80*spin_direction);
  for(i = 0; i<250; i++){
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
```
Once the line sensor is calibrated you can use `readLine()` to get the position of the line. This function
requires that you have an `unsigned int array` to store the line sensor data. In the example code below, I'm
using the ZumoBuzzer to play a tone that gets higher when the line is to the right. This is a nice method
for checking that it is working with using the `Serial` port (and thus tethering the robot to your computer).

```c++
ZumoBuzzer buzzer; // create a buzzer to play sound
unsigned int sensor_vals[6]; // initialize an array for the 6 values from the sensor
void loop(){
  int line_position = readLine(sensor_vals);
  
}
```
