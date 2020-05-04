#include <Wire.h>
#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray linesensors(QTR_NO_EMITTER_PIN);
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);

void setup() {
  int i;
  int spin_direction = 1;
  motors.setSpeeds(120*spin_direction, -120*spin_direction);
  for(i = 0; i<80; i++){
    linesensors.calibrate();
    if(i%40 == 20){ // every 50 loops, starting on loop 25...
      spin_direction = -spin_direction;
      motors.setSpeeds(120*spin_direction, -120*spin_direction);
    }
    delay(10);
  }
  motors.setSpeeds(0,0);
  delay(500);
}

unsigned int sensor_vals[6];
int THRESHOLD = 400;
int BASE_SPEED = 200;
int line_position;
char path[50];
int turn_counter = 0;
bool finish_detected = false;

int INTERSECTION_LEFT_TURN = 0;
int INTERSECTION_RIGHT_TURN = 1;
int INTERSECTION_T = 2;
int INTERSECTION_CROSS = 3;
int INTERSECTION_LEFT_T = 4;
int INTERSECTION_RIGHT_T = 5;
int FINISH = 6;
int NO_INTERSECTION = 7;

void loop() {
  line_position = linesensors.readLine(sensor_vals);
  
  bool line_on_left = sensor_vals[0] > THRESHOLD;
  bool line_on_right = sensor_vals[5] > THRESHOLD;

  int intersection_type = NO_INTERSECTION;
  
  if(line_on_left || line_on_right) {
    motors.setSpeeds(BASE_SPEED, BASE_SPEED);
    intersection_type = get_intersection_type();
  }

  if(intersection_type == NO_INTERSECTION && sensor_vals[1] < THRESHOLD && sensor_vals[2] < THRESHOLD && sensor_vals[3] < THRESHOLD && sensor_vals[4] < THRESHOLD){
    path[turn_counter] = 'U';
    turn_counter++;
    u_turn();
  }
  
  if(intersection_type == INTERSECTION_RIGHT_T){
    path[turn_counter] = 'S';
    turn_counter++;
    follow_line();
  }
  if(intersection_type == INTERSECTION_LEFT_TURN || 
     intersection_type == INTERSECTION_LEFT_T || 
     intersection_type == INTERSECTION_CROSS || 
     intersection_type == INTERSECTION_T){
    path[turn_counter] = 'L';
    turn_counter++;
    turn_left();
  } 
  if(intersection_type == INTERSECTION_RIGHT_TURN){
    path[turn_counter] = 'R';
    turn_counter++;
    turn_right();
  } 
  if(intersection_type == NO_INTERSECTION) {
    follow_line();
  }
  if(intersection_type == FINISH){
    solved();
  }
}

int get_intersection_type() {
  bool line_on_left = sensor_vals[0] > THRESHOLD;
  bool line_on_right = sensor_vals[5] > THRESHOLD;
  
  bool line_on_left_ever = line_on_left;
  bool line_on_right_ever = line_on_right;

  int finish_counter = 0;
  
  while(line_on_left || line_on_right){
    linesensors.read(sensor_vals);
    line_on_left = sensor_vals[0] > THRESHOLD;
    line_on_right = sensor_vals[5] > THRESHOLD;

    line_on_left_ever = line_on_left || line_on_left_ever;
    line_on_right_ever = line_on_right || line_on_right_ever;

    finish_counter++;
    if(finish_counter > 60){
      //buzzer.play("O6c32d32e32f32g32");
      return FINISH;
    }
  }
  
  bool line_straight = sensor_vals[2] > THRESHOLD || sensor_vals[3] > THRESHOLD;

  if(!line_straight && line_on_left_ever && line_on_right_ever) {
    buzzer.play("O6c32c32c32");
    return INTERSECTION_T;
  }
  if(line_straight && line_on_left_ever && line_on_right_ever){
    buzzer.play("O6c32e32c32");
    return INTERSECTION_CROSS;
  }
  if(!line_straight && !line_on_right_ever){
    buzzer.play("O6c32d32e32");
    return INTERSECTION_LEFT_TURN;
  }
  if(!line_straight && !line_on_left_ever){
    buzzer.play("O6e32d32c32");
    return INTERSECTION_RIGHT_TURN;
  }
  if(line_straight && !line_on_right_ever){
    buzzer.play("O6e32d32e32");
    return INTERSECTION_LEFT_T;
  }
  if(line_straight && !line_on_left_ever){
    buzzer.play("O6c32d32c32");
    return INTERSECTION_RIGHT_T;
  }
}

void turn_left() {
  motors.setSpeeds(-BASE_SPEED, BASE_SPEED);
  while(sensor_vals[3] > THRESHOLD){
    linesensors.read(sensor_vals);
  }
  while(sensor_vals[3] < THRESHOLD){
    linesensors.read(sensor_vals);
  }
}

void turn_right() {
  motors.setSpeeds(BASE_SPEED, -BASE_SPEED);
  while(sensor_vals[2] > THRESHOLD){
    linesensors.read(sensor_vals);
  }
  while(sensor_vals[2] < THRESHOLD){
    linesensors.read(sensor_vals);
  }
}

void u_turn() {
  motors.setSpeeds(-BASE_SPEED, BASE_SPEED);
  while(sensor_vals[0] < THRESHOLD){
    line_position = linesensors.readLine(sensor_vals);
  }
  while(line_position > 3000 || line_position < 2000){
    line_position = linesensors.readLine(sensor_vals);
  }
}

void solved(){
  motors.setSpeeds(0,0);
  path_reduce();
  for(int i=0; i<turn_counter+1; i++){
    if(path[i] == 'L'){
      buzzer.playNote(NOTE_C(6), 100, 15);
      delay(200);
    }
    if(path[i] == 'R'){
      buzzer.playNote(NOTE_E(6), 100, 15);
      delay(200);
    }
    if(path[i] == 'S'){
      buzzer.playNote(NOTE_G(6), 100, 15);
      delay(200);
    }
    if(path[i] == 'U'){
      buzzer.playNote(NOTE_C(7), 100, 15);
      delay(200);
    }
  }
  
  button.waitForButton();

  buzzer.play("O6 d32a32");
  delay(1000);
  
  runSolvedMaze();
}

double PROPORTION_GAIN = 0.2;
double DERIVATIVE_GAIN = 3;
int last_error = 0;
void follow_line(){
  // follow line
  int error = line_position - 2500;
  int error_change = error - last_error;
  int left_speed = BASE_SPEED + PROPORTION_GAIN * error + DERIVATIVE_GAIN * error_change;
  int right_speed = BASE_SPEED + -PROPORTION_GAIN * error + -DERIVATIVE_GAIN * error_change;
  last_error = error;

  // set speed
  motors.setSpeeds(left_speed, right_speed);
}


void runSolvedMaze(){
  int solved_path_location = 0;
  while(true){
    line_position = linesensors.readLine(sensor_vals);
  
    bool intersection_detected = sensor_vals[0] > THRESHOLD || sensor_vals[5] > THRESHOLD;
    
    if(intersection_detected) {
      if(solved_path_location == turn_counter){
        motors.setSpeeds(0,0);
        buzzer.play("!T250 O4 L8 g O5 ceg O6 ceg4.e4. O4 a- O5 ce-a- O6 ce-a-4.e-4. O4 b- O5 dfb- O6 df b-4r MS b-b-b- ML O7 c2.");
        break;
      }
      
      motors.setSpeeds(BASE_SPEED, BASE_SPEED);
      get_intersection_type(); // just to get through the intersection
    
      if(path[solved_path_location] == 'S'){
        follow_line();
      }
      if(path[solved_path_location] == 'L'){
        turn_left();
      } 
      if(path[solved_path_location] == 'R'){
        turn_right();
      } 
      
      solved_path_location++;
    } else {
      follow_line();
    }
  }
  button.waitForButton();

  buzzer.play("O6 d32a32");
  delay(1000);
  
  runSolvedMaze();
}

void path_reduce(){
  bool flag = false;  
  for(int path_location=2; path_location < turn_counter; path_location++){
    if(flag){
      path[path_location-2] = path[path_location];
    } else if(path[path_location-1] == 'U'){
      char a = path[path_location - 2];
      char b = path[path_location];
      char c = ' ';
      int total_angle = 180;
      if(a == 'L'){
        total_angle += 270;
      }
      if(a == 'U'){
        total_angle += 180;
      }
      if(a == 'R'){
        total_angle += 90;
      }
      if(b == 'L'){
        total_angle += 270;
      }
      if(b == 'U'){
        total_angle += 180;
      }
      if(b == 'R'){
        total_angle += 90;
      }
      total_angle = total_angle % 360;
      if(total_angle == 0){
        c = 'S';
      }
      if(total_angle == 90){
        c = 'R';
      }
      if(total_angle == 180){
        c = 'U';
      }
      if(total_angle == 270){
        c = 'L';
      }
      path[path_location - 2] = c;
      flag = true;
    }
  }
  if(flag){
    turn_counter = turn_counter - 2;
    path[turn_counter] = ' ';
    path[turn_counter + 1] = ' ';
    path_reduce();
  } else {
    return;
  }
}
