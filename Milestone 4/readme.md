# Milestone 4

## Solving a maze with no branching

As mentioned in the tutorial content for Milestone 3, your line-following robot should be capable of solving this task with little or no modification. The only necessary change is to determine when the robot reaches the end of the maze. The end is represented by a solid black rectangle (three lines thick) that is more than the width of the robot. 

Eventually we will need to distinguish between a T intersection or a + intersection and the end of the maze. For now, though, we don't really
need to worry about that since this maze is just one path with no intersections. Therefore, we can just check if the outer-most sensors are 
detecting the line at the same time.

```c++
void loop() {
  line_position = linesensors.readLine(sensor_vals);
  if(sensor_vals[0] > THRESHOLD && sensor_vals[5] > THRESHOLD){
    solved(); 
  }else if(sensor_vals[0] > THRESHOLD && sensor_vals[1] > THRESHOLD){
    turn_left();
  } else if(sensor_vals[4] > THRESHOLD && sensor_vals[5] > THRESHOLD){
    turn_right();
  } else {
    follow_line();
  }
}
```

If we reach this state, then the `solved()` function is called. This could stop the robot and then do nothing:

```c++
void solved(){
  motors.setSpeeds(0,0);
  while(true){ 
    // do nothing so that the main loop() doesn't restart.
  }
}
```

## Recording the path through the maze

Here I'll focus on something that you'll want to be able to do eventually: remember the path through the maze.

One way to do this is by storing each turn that the robot takes. I'll use a `char` array to do this. Every time the
robot makes a turn I'll store the turn direction in the `path` array.

Using the same code as Milestone 3, but now with a little bit more functionality to store the turn-by-turn list:

```c++
char path[50];
int turn_counter = 0;
void loop() {
  line_position = linesensors.readLine(sensor_vals);
  if(sensor_vals[0] > THRESHOLD && sensor_vals[5] > THRESHOLD){
    solved(); 
  }else if(sensor_vals[0] > THRESHOLD && sensor_vals[1] > THRESHOLD){
    path[turn_counter] = 'L';
    turn_counter++;
    turn_left();
  } else if(sensor_vals[4] > THRESHOLD && sensor_vals[5] > THRESHOLD){
    path[turn_counter] = 'R';
    turn_counter++;
    turn_right();
  } else {
    follow_line();
  }
}
```

Note the use of `turn_counter` to keep track of which step the robot is at in the maze. Also, note the use of **single quotes** instead
of double quotes for the `'L'` and `'R'` string. Single quotes and double quotes have different meanings in C. Single quotes are meant 
for single characters.

Finally, to do something with the `path` information we can play a little tune when the robot is finished, with different notes representing
different turn directions.

```c++
void solved(){
  motors.setSpeeds(0,0);
  for(int i=0; i<turn_counter+1; i++){
    if(path[i] == 'L'){
      buzzer.playNote(NOTE_G(5), 200, 15);
      delay(400);
    }
    if(path[i] == 'R'){
      buzzer.playNote(NOTE_G(6), 200, 15);
      delay(400);
    }
  }
  while(true){
    // do nothing!
  }
}
```

Here's a video of the robot solving a maze and playing the tune.

[![YouTube Video](https://img.youtube.com/vi/Q6VOK72gTTk/0.jpg)](https://youtu.be/Q6VOK72gTTk)



