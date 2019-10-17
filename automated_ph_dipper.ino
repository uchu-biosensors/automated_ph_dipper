#include <Servo.h>

// Timing variables
#define rotation_degrees 30
#define vertical_degrees 50
#define step_delay       30
#define sensing_delay    2000
#define mvmt_delay       250

// Button Pins
#define setup_btn        8
#define sequence_btn     9
#define end_seq_btn      10
#define ret_to_setup_btn 11

// Setup moves from 0 to 'vertical_degrees', (0,30)
void goto_setup_pos();
// Moves arm to continually dip sensor in beakers
void begin_test_seq();
// Moves from curr_horz_pos and curr_vert_pos to (0,0)
void goto_start_pos();

// Moves arm in increments of 'rotation_degrees' and 'vertical_degrees'
void move_up();
void move_down();
void rotate_left();
void rotate_right();

// Servo motor objects
Servo myservo_v;  
Servo myservo_h;  

// Global variables to track current position
int curr_horz_pos = 0;
int curr_vert_pos = 0; 

/*  *  *  *  *  *  *  *  *
 *  Setup and main loop 
 *  *  *  *  *  *  *  *  */
 
void setup() {
  Serial.begin(9600);
  myservo_v.write(0);
  myservo_h.write(0);
  myservo_v.attach(2); 
  myservo_h.attach(3); 
  pinMode(setup_btn,        INPUT);
  pinMode(sequence_btn,     INPUT);
  pinMode(end_seq_btn,      INPUT);
  pinMode(ret_to_setup_btn, INPUT);
}

void loop() {
  if (digitalRead(setup_btn) == HIGH)
    goto_setup_pos();
  if (digitalRead(sequence_btn) == HIGH) {
    begin_test_seq();
  }
  if (digitalRead(ret_to_setup_btn) == HIGH) {
    goto_start_pos();
  }
}

/*  *  *  *  *  *  *  *  *
 *  End setup and main loop
 *  *  *  *  *  *  *  *  */
 
void goto_setup_pos() {
  // Move vertical to correct position
  for (curr_vert_pos; curr_vert_pos <= vertical_degrees; curr_vert_pos++) { 
    myservo_v.write(curr_vert_pos);            
    delay(step_delay);      
  }    
  // Move horizontal to correct position
  for (curr_horz_pos; curr_horz_pos >= 0; curr_horz_pos--) { 
    myservo_h.write(curr_horz_pos);            
    delay(step_delay);      
  } 
}

bool sense_ph() {
  unsigned long start = millis();
  unsigned long curr = start;
  while (int(curr - start) <= sensing_delay) {
    if (digitalRead(end_seq_btn) == HIGH) {
      return true;
    }
    curr = millis();
  }
  return false;
}

  // Sequence: beaker 1, beaker 2, beaker 3, beaker 2, beaker 1, repeat
void begin_test_seq() {
  bool seq_ended = false;
  while (!seq_ended) {
    // 1 --> 2 --> 3
    move_down();
    if (sense_ph()) break;
    move_up();
    rotate_left();
    move_down();
    if (sense_ph()) break;
    move_up();
    rotate_left();
    move_down();
    if (sense_ph()) break;
    move_up();
  
    // 3 --> 2--> 1
    rotate_right();
    move_down();
    if (sense_ph()) break;
    move_up();
    rotate_right(); 
  }
  goto_setup_pos();
}

// Return to (0,0)
void goto_start_pos() {
  // Move vertical to correct position
  for (curr_vert_pos; curr_vert_pos >= 0; curr_vert_pos--) { 
    myservo_v.write(curr_vert_pos);            
    delay(step_delay);      
  }    
  // Move horizontal to correct position
  for (curr_horz_pos; curr_horz_pos >= 0; curr_horz_pos--) { 
    myservo_h.write(curr_horz_pos);            
    delay(step_delay);      
  }      
  bool return_to_setup = false;
  while (!return_to_setup) {
    if (digitalRead(ret_to_setup_btn) == HIGH) {
      move_down();
      return_to_setup = true;
    }
  }
}

void move_up() {
  int new_pos = curr_vert_pos + vertical_degrees;
  // Check position is not greater than 180 degrees
  if (new_pos <= 180) {
    for (curr_vert_pos; curr_vert_pos <= new_pos; curr_vert_pos++) { 
      myservo_v.write(curr_vert_pos);            
      delay(step_delay);      
    }   
    delay(mvmt_delay);
  }
}

void move_down() {
  int new_pos = curr_vert_pos - vertical_degrees;
  // Check position is not less than 0 degrees
  if (new_pos >= 0) {
    for (curr_vert_pos; curr_vert_pos >= new_pos; curr_vert_pos--) { 
      myservo_v.write(curr_vert_pos);             
      delay(step_delay);                       
    } 
    delay(mvmt_delay);
  }
}

void rotate_left() {
  int new_pos = curr_horz_pos + rotation_degrees;
  // Check position is not more than 180 degrees
  if (new_pos <= 180) {
    for (curr_horz_pos; curr_horz_pos <= new_pos; curr_horz_pos++) { 
      myservo_h.write(curr_horz_pos);            
      delay(step_delay);      
    }   
    delay(mvmt_delay);
  }
}

void rotate_right() {
  int new_pos = curr_horz_pos - rotation_degrees;
    for (curr_horz_pos; curr_horz_pos >= new_pos; curr_horz_pos--) { 
      myservo_h.write(curr_horz_pos);            
      delay(step_delay);      
    }    
    delay(mvmt_delay);
}
