# Milestone 3

## Using the line sensor to detect and respond to turns

If you've developed code for Milestone 2 and tried it out on the square for this milestone, then you've probably found that
your code already works. A good line-following algorithm, like the PD-control implemented in the last tutorial is capable of
navigating the sharp turns of the maze.

In fact, you could probably solve Milestone 4 with the same code, too.

Still, it will be useful when we get to Milestone 5 to think about how to detect turns in the course. So rather than just 
solving the square with the existing line following code, this tutorial aims to show you a way to use the line sensor array
to detect the presence of turns.

The key idea is that when you call `.readLine()` this returns the position of the line but also updates the array of line
sensor values. You can use this array to detect a situation in which the line is thicker than expected, and determine
whether the thicker line goes to the left or the right.

Here's an example:

```c++
unsigned int sensor_vals[6];
int THRESHOLD = 600;
int line_position;

void loop() {
  line_position = linesensors.readLine(sensor_vals);
  
  if(sensor_vals[0] > THRESHOLD && sensor_vals[1] > THRESHOLD){
    turn_left();
  } else if(sensor_vals[4] > THRESHOLD && sensor_vals[5] > THRESHOLD){
    turn_right();
  } else {
    follow_line();
  }
}
```

In the above `loop()` we check the current position of the line, which also updates the `sensor_vals` array. If the two left-most
sensors are above some threshold (detecting the line) then we treat that as the line being on the left. Conversely, if the two
right-most sensors are above the threshold then we assume the line is on the right. If neither condition is true, continue following
the line.

## Turning left and right: two methods

There are two general strategies that you could take for turning the robot to the left and right, and they illustrate different 
ways of programming behavior in a robot.

One strategy is the **ballistic approach** and the other is the **reactive approach**.

### Ballistic Approach

A ballistic behavior is a behavior that executes regardless of any input to the system. Once the behavior starts it cannot be stopped
until it is complete. Ballistic behaviors are sometimes useful, and they are certainly the simple to implement. 

To implement a ballistic turning behavior, we could program the robot to spin the motos in the direction that accomplishes the turn
for a fixed amount of time. The code would look something like this:

```c++
void turn_left() {
  motors.setSpeeds(-400, 400);
  delay(115);
}
```

In order to get the ballistic behavior to work well, you'll need to tune the parameters: the speed that the motors spin and the length of the delay. This can be done without too much trial-and-error. 

One problem that you might encounter with a ballistic behavior is that **performance changes over time**. Batteries are drained, treads become
less grippy, the floor becomes dirtier, etc. All of these small changes can have an impact on the success of your ballistic behavior, so you 
may need to re-tune the parameters regularly.

Here's a video of the robot using the ballistic approach:

![](img/ballistic_square.gif)

### Reactive Approach

A reactive behavior is responsive to some kind of sensory signal. The line-following behavior is a nice example of a reactive behavior.

We can also make the turning behavior reactive. We can use the line sensors to detect when we have turned enough to proceed. Note that turning
enough is not necessarily turning 90 degrees. If we get the robot to turn so that the line-following algorithm will kick in and allow the robot to follow the path then that should be enough.

The code could look like this:

```c++
void turn_left() {
  motors.setSpeeds(-BASE_SPEED, BASE_SPEED);
  linesensors.read(sensor_vals);
  while(sensor_vals[0] > THRESHOLD && sensor_vals[1] > THRESHOLD){
      linesensors.read(sensor_vals);
  }  
}
```

Essentially: turn until the robot doesn't detect the line on its left. And then kick the control back to the line-following algorithm in the `loop()`. 

Note that there are parameters to play with here as well. The speed of the motors can be tweaked for optimal performance.

Here's a video of the robot using the reactive approach:

![](img/reactive_square.gif)

### A Hybrid Approach?

You might find some benefit in combining the reactive and ballistic approaches into a Hybrid approach. For example, the reactive approach 
angles the robot towards the correct direction. It *might* be faster if the reactive approach turned just a little bit further. This could
be done by adding a small amount of ballistic turning onto the end of the reactive `turn_left()` function.

## Code

I'll encourage you to use the examples above to implement your own solution. You are welcome to view my ballistic and reactive solutions
in this folder when you are ready to do so. They are certainly not optimal solutions and you can work to find some ways to speed it up.
