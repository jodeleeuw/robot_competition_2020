# Milestone 2

## Tutorial: Using the Zumo Line Sensor Array

The Zumo shield has an array of six short-range IR reflectance sensors under the front of the robot. 

![](img/line_sensors.jpg)

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

*Imporant: You must connect the jumper for the buzzer before it will play sound. See [this video](https://www.youtube.com/watch?v=SL-j1g6T6WY) for instructions.*

```c++
ZumoBuzzer buzzer; // create a buzzer to play sound
unsigned int sensor_vals[6];
void loop() {
  int line_position = linesensors.readLine(sensor_vals);
  int frequency = 220 + ((float)line_position / 5000) * 660;
  buzzer.playFrequency(frequency, 100, 15);
  while (buzzer.isPlaying());
}
```

The full code for this method is available in the `read_line_method` folder.

#### Option 2: Raw Data

You can also read the raw values for each sensor using the `read()` method. This method does not require any
calibration.

There's not an interesting way that I could think of to provide access to the values, so I use the normal
`Serial` port method for communicating with the Arduino.

```c++
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
```

Notice that calling `read()` does not return any new values. Instead it updates the values of the `sensor_vals`
array. 

If you upload this code and open your serial monitor (making sure to set the Baud rate to 9600 in the bottom
corner of the serial monitor) you should see lines of numbers with 6 items per line. The values should range 
from 0 to 2000, with 2000 being when the sensor is directly over the line.

You could save yourself many lines of code in the `loop()` by using the `sprintf()` function. If you want
to adapt the code you can Google `sprintf` to learn how to use it.

This code is also available in the `raw_data_method` folder.

## Tutorial: PID control

Once you have the ability to get the position of the line you are ready to create a line-following robot.

[This 5 minute video](https://www.youtube.com/watch?v=4Y7zG48uHRo) does an excellent job of explaining 
the basics of **PID Control**, which provides a solid foundation for building the line-following mechanism. 
I recommend watching it and then coming back to the turorial.

For all of these methods we need a way of measuring the *error* in the position of the robot relative
to the line. We can use the `readLine()` method described above to get the error. If the robot is pefectly
centered over the line the `readLine()` method should return `2500`. Therefore...

```c++
int line_position = linesensors.readLine(sensor_vals);
int error = line_position - 2500;
```

... works as a measurement of *error*. Positive errors indicate that the robot is too far to the left of the line.
Negative errors indicate the robot is too far to the right of the line.

### Bang-bang control

Let's start with the simplest case in the video: *bang-bang* control. For bang-bang control, we have a fixed
speed that we adjust the robot by when it is not centered over the line. 

Here's what a the `loop()` might look like:

```c++
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
```

If the error is positive (and above some minimum error threshold) the robot will drift to the right. If
the error is negative the robot will drift to the left. If the error is within some tolerance the robot
moves straight.

The result is an OK line-follower, but it has a hard time correcting for curves:

![](img/bb_lf.gif)

You could try tuning parameters here like the motor speeds and the tolerance for error. Setting the error
tolerance lower should make for better line following, but the behavior of the robot will become very
jerky.

See the `bang_bang_control` folder for this code.

### Proportional control

As described in the [video on PID control](https://www.youtube.com/watch?v=4Y7zG48uHRo), one solution
to improve the smoothness of the line tracking is to use the *amount* of error to adjust the *amount* of
correction. The larger the error, the more correction we apply. This is called **proportion-based** control.

Here's an example of applying proportion-based control.

```c++
unsigned int sensor_vals[6];
int BASE_SPEED = 200;
double PROPORTION_GAIN = 0.2;
void loop() {
  int line_position = linesensors.readLine(sensor_vals);
  int error = line_position - 2500;
  int left_speed = BASE_SPEED + PROPORTION_GAIN * error;
  int right_speed = BASE_SPEED + -PROPORTION_GAIN * error;
  motors.setSpeeds(left_speed, right_speed);
}
```

The `PROPORTION_GAIN` variable scales how much the error signal changes the motor signal. For example,
if the line sensor reads `3000`, the value for `left_speed` will be `300` and the value for `right_speed`
will be `100`. Note that the sign on `PROPORTION_GAIN` is negative for the right motor.

Setting the right value for `PROPORTION_GAIN` is crucial for success.

Here's what the robot looks like with a value of `1.0`

![](img/pc_high.gif)

Here's what the robot looks like with a value of `0.2`

![](img/pc_tuned.gif)

Here's what the robot looks like with a value of `0.02`

![](img/pc_low.gif)

### PD control

Taking into account the *rate* at which the error is changing can allow for smoother control. When
the error is changing quickly, we slow down the amount of adjustment. This lets the system smoothly
stabilize. Adding this *derivative* term into the controller produces a **PD (proportion + derivative)** 
controller.

Here's an example of adding the derivative term to the proportion-based controller above.

```c++
unsigned int sensor_vals[6];
int BASE_SPEED = 200;
double PROPORTION_GAIN = 0.2;
double DERIVATIVE_GAIN = 3;
int last_error = 0;
void loop() {
  int line_position = linesensors.readLine(sensor_vals);
  int error = line_position - 2500;
  int error_change = error - last_error;
  int left_speed = BASE_SPEED + PROPORTION_GAIN * error + DERIVATIVE_GAIN * error_change;
  int right_speed = BASE_SPEED + -PROPORTION_GAIN * error + -DERIVATIVE_GAIN * error_change;
  last_error = error;
  motors.setSpeeds(left_speed, right_speed);
}
```
Like the proportion-based term, there is a `DERIVATIVE_GAIN` parameter that we can tune to improve
the performance of the robot. However, the proportion-based controller by itself was already pretty
good with a reasonable parameter, so setting `DERIVATIVE_GAIN` to be very small just makes the robot
behave like it is proportion-only. 

With a value of `3.0` the robot's performance is very smooth.

![](img/pd_tuned.gif)

When the value of `DERIVATIVE_GAIN` is too high, you get very tight line following but the robot oscillates
quickly over the line. Here it is set to `30`. Notice that the robot is travelling slower here
than in the above clip, because of the oscillations.

![](img/pd_high.gif)

### PID Control

The final option would be to add a term that is responsive to the sum of errors, the *integral* term. 
However, PD-control alone is more than sufficient for this task, in part because the robot is 
so responsive and errors can be corrected very quickly. So we don't need to implement the I-portion of
the PID controller.






