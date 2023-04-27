#include "SimpleRSLK.h"
#include <Servo.h>

Servo feederServo;


const int motorbias = 0;

//variables declared for state 1
const int motor3pwmpin = 40;
const int motor3dir1pin = 39;
const int motor3dir2pin = 38;
const int motor4dir1pin = 37;
const int motor4dir2pin = 36;
const int motor4pwmpin = 35;
const int motor1pwmpin = 19;
const int motor1dir1pin = 18;
const int motor1dir2pin = 17;
const int motor2dir1pin = 15;
const int motor2dir2pin = 14;
const int motor2pwmpin = 11;

//variables declared for state 1
int trigPin1 = 24;
int trigPin2 = 26;
int echoPin1 = 23;
int echoPin2 = 25;
long duration1;
long distance1;
long duration2;
long distance2;
long deltaDistance;
unsigned long startTime;
unsigned long currentTime;
int squareCounter = 0;

//variables and declarations for state 2 RSLK library webpage https://fcooper.github.io/Energia-RSLK-Library/_simple_r_s_l_k_8h.html#afa1b08c477adccb29dc3ad97a58b63c7

bool isCalibrationComplete = false;
uint16_t sensorVal[LS_NUM_SENSORS];  //LS_NUM_SENSORS is the value 8
uint16_t sensorCalVal[LS_NUM_SENSORS];
uint16_t sensorMaxVal[LS_NUM_SENSORS];
uint16_t sensorMinVal[LS_NUM_SENSORS];
int TimeHitLine = 0;
int TimeSinceHitLine = 0;
int linesHit = 0;

uint8_t lineColor = DARK_LINE;

//variables and declarations for state 3
//pins
int IRbeaconLeft = 5;
int IRbeaconMiddle = 6;
int IRbeaconRight = 8;
int direction, navpwm, t0;

float kpnav = .34;


const int shooterpin = 30;
const int shoot_duration = 100; // milliseconds of how long to keep the switch open

const int servopin = 9;

enum states {  // list all states
  NONE,
  ORIENT,
  BASKET,  // state in finding the basket
  SHOOT,
  NAVIGATE

};

states state, prior_state;


void setup() {

  //setup values for state 2, the reason why this is done first is that setupRSLK() declares a bunch of stuff, and a lot of it we want overwritten.
  Serial.begin(115200);

  setupRSLK();
  /* Left button on Launchpad */
  setupWaitBtn(LP_RIGHT_BTN);
  /* Red led in rgb led */
  setupLed(RED_LED);

  clearMinMax(sensorMinVal, sensorMaxVal);

  //motor designations: 2 1
  //                    3 4



  //state 1 pins
  //Ultrasonic sensor designations: sensor 1 is left, sensor 2 is right.
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  startTime = micros();  //Timer used for ultrasonic sensors


  //state 3 pins and setup
  pinMode(IRbeaconLeft, INPUT);
  pinMode(IRbeaconMiddle, INPUT);  //in the demo these are pullups
  pinMode(IRbeaconRight, INPUT);

  pinMode(shooterpin, OUTPUT);

  digitalWrite(shooterpin, LOW);

  

  feederServo.attach(servopin);

  prior_state = NONE;
  state = ORIENT;
}


void loop() {

  //before entering our state machine, we need to calibrate our photoresistors.

  /* Run this setup only once */
  if (isCalibrationComplete == false) {
    floorCalibration();
    isCalibrationComplete = true;
  }


  switch (state) {

    case ORIENT:
      orient();
      break;

    case NAVIGATE:
      navigate();
      break;

    case BASKET:
      basket();
      break;
    
    case SHOOT:
      shoot();
      break;

   
  }

}  //loop ends here


/////////////////////////////////following 2 functions are used to calibrate the photoresistors///////////////////////
void floorCalibration() {
  /* Place Robot On Floor (no line) */
  delay(2000);
  String btnMsg = "Push left button on Launchpad to begin calibration.\n";
  btnMsg += "Make sure the robot is on the floor away from the line.\n";
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_RIGHT_BTN, btnMsg, RED_LED);

  delay(500);

  Serial.println("Running calibration on floor");
  simpleCalibrate();
  Serial.println("Reading floor values complete");

  btnMsg = "Push left button on Launchpad to begin line following.\n";
  btnMsg += "Make sure the robot is on the line.\n";
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_RIGHT_BTN, btnMsg, RED_LED);
  delay(1000);

  //enableMotor(BOTH_MOTORS); leftover from RSLK code, dont think it has a use.
}

void simpleCalibrate() {

  //in the rslk code, motors run foward for a second here, I dont know if we need to do that.

  for (int x = 0; x < 100; x++) {
    readLineSensor(sensorVal);
    setSensorMinMax(sensorVal, sensorMinVal, sensorMaxVal);
  }

  // in RSLK, all motors are disabled here, IDK if we need to do that.
}

/////////////////////////////////above 2 functions are used to calibrate the photoresistors///////////////////////








void drive(int i, int pwm) {  // drive(1) means drive forward
  switch (i) {
    case 1:  // drive forward
      set_direction(1, 1);
      set_direction(2, 1);
      set_direction(3, 1);
      set_direction(4, 1);

      break;
    case 2:  // strafe left
      set_direction(1, 1);
      set_direction(2, -1);
      set_direction(3, 1);
      set_direction(4, -1);
      break;
    case 3:  // strafe right
      set_direction(1, -1);
      set_direction(2, 1);
      set_direction(3, -1);
      set_direction(4, 1);
      break;
    case 4:  // reverse
      set_direction(1, -1);
      set_direction(2, -1);
      set_direction(3, -1);
      set_direction(4, -1);
      break;
    case 5:  //rotate in place to the left
      set_direction(1, -1);
      set_direction(2, 1);
      set_direction(3, 1);
      set_direction(4, -1);
  }

  if (pwm != 0){
  analogWrite(motor1pwmpin, pwm);
  analogWrite(motor2pwmpin, pwm+motorbias);
  analogWrite(motor3pwmpin, pwm+motorbias);
  analogWrite(motor4pwmpin, pwm);
  }
  else{
    analogWrite(motor1pwmpin, pwm);
    analogWrite(motor2pwmpin, pwm);
    analogWrite(motor3pwmpin, pwm);
    analogWrite(motor4pwmpin, pwm);
  }
}


void set_direction(int i, int k) {
  switch (i) {
    case 1:
      if (k > 0) {
        digitalWrite(motor1dir1pin, HIGH);
        digitalWrite(motor1dir2pin, LOW);
      } else if (k < 0) {
        digitalWrite(motor1dir1pin, LOW);
        digitalWrite(motor1dir2pin, HIGH);
      } else {
        digitalWrite(motor1dir1pin, HIGH);
        digitalWrite(motor1dir2pin, HIGH);
      }
      break;

    case 2:
      if (k > 0) {
        digitalWrite(motor2dir1pin, HIGH);
        digitalWrite(motor2dir2pin, LOW);
      } else if (k < 0) {
        digitalWrite(motor2dir1pin, LOW);
        digitalWrite(motor2dir2pin, HIGH);
      } else {
        digitalWrite(motor2dir1pin, HIGH);
        digitalWrite(motor2dir2pin, HIGH);
      }
      break;
    case 3:
      if (k > 0) {
        digitalWrite(motor3dir1pin, HIGH);
        digitalWrite(motor3dir2pin, LOW);
      } else if (k < 0) {
        digitalWrite(motor3dir1pin, LOW);
        digitalWrite(motor3dir2pin, HIGH);
      } else {
        digitalWrite(motor3dir1pin, HIGH);
        digitalWrite(motor3dir2pin, HIGH);
      }
      break;
    case 4:
      if (k > 0) {
        digitalWrite(motor4dir1pin, HIGH);
        digitalWrite(motor4dir2pin, LOW);
      } else if (k < 0) {
        digitalWrite(motor4dir1pin, LOW);
        digitalWrite(motor4dir2pin, HIGH);
      } else {
        digitalWrite(motor4dir1pin, HIGH);
        digitalWrite(motor4dir2pin, HIGH);
      }
      break;
  }
}



void basket() {
  //state 3, robot has reached close line. BEHAVIOR: robot moves in the direction of most recently active active side IR sensor.
  //EXIT CASE: both side IR sensors return 0, front IR sensor returns 1, and all photoresistors return 1. Exits to state 4

  // the point of the direction variable is to store the direction that the car was moving most recently. This comes into effect when the middle IR sensor is the only one sensing something
  //but we still need to go in a direction.

  if (prior_state != state) {
    prior_state = BASKET;
  }

  int irLeft = digitalRead(IRbeaconLeft);
  int irMiddle = digitalRead(IRbeaconMiddle);
  int irRight = digitalRead(IRbeaconRight);

  Serial.println();

  if (irLeft == 1) {

    //motor 1 backward
    //motor 4 backward
    //motor 2 forward
    //motor 3 forward
    direction = 2;
  }

  if (irRight == 1) {

    //motor 1 forward
    //motor 4 forward
    //motor 2 backward
    //motor 3 backward
    direction = 3;
  }

  if (irMiddle == 1) {
    if (direction == 2) {  //this means the most recent direction that was sensed was left.
      //motor 1 backward
      //motor 4 backward
      //motor 2 forward
      //motor 3 forward
      drive(3,255); // strafe right
    }
    if (direction == 3) {  //this means the most recent direction sensed was right.
      //motor 1 forward
      //motor 4 forward
      //motor 2 backward
      //motor 3 backward
      drive(2,255); // strafe left
    }

    readLineSensor(sensorVal);

    readCalLineSensor(sensorVal,
                      sensorCalVal,
                      sensorMinVal,
                      sensorMaxVal,
                      lineColor);

    if (sensorVal[0] > 2000 && sensorVal[1] > 2000 && sensorVal[2] > 2000 && sensorVal[3] > 2000 && sensorVal[4] > 2000 && sensorVal[5] > 2000 && sensorVal[6] > 2000 && sensorVal[7] > 2000) {
      //if thise condition is satisfied, we are right on top of a cross.

      state = SHOOT;
    }

  }  //end of the middle check statement

  if (prior_state != state) {
    drive(1, 0);
  }
}

void navigate() {

  if (prior_state != state) {
    prior_state = NAVIGATE;
    linesHit = 0;
  }

  //state 2, robot has squared itself to the backbaord. BEHAVIOR: robot translates forward.
  //EXIT CASE: HasHitLine=2, meaning robot is on closest line to hoops, can only exit to state 3
  //NOTE: line hit is defined as both middle photoresitors returning black
  //drive(1, 0);
  //delay(500);     //delay .5 seconds, this might not be needed
  drive(1, 210);  //go straight, we want to go fast.

  //Get sensor array data and compute position of line
  readLineSensor(sensorVal);
  readCalLineSensor(sensorVal,
                    sensorCalVal,
                    sensorMinVal,
                    sensorMaxVal,
                    lineColor);  //this code updates the sensors min and max values based on the current data. SOMETHING TO TEST??
  uint32_t linePos = getLinePosition(sensorCalVal, lineColor);


  //
  // Serial.println(sensorVal[4]);
  //  Serial.println("---------------------");
  int sensorvalue4 = sensorVal[4];

  //Serial.println(TimeSinceHitLine);
  if (sensorvalue4 > 1500 && linesHit == 0) {  //the less than statement here should make it such there there is a 1 second delay until a linehit will register again
    //Serial.println("hit the first line");
    TimeHitLine = millis();
    linesHit = 1;
    TimeSinceHitLine = millis() - TimeHitLine;
  }


  Serial.println(TimeSinceHitLine);
  Serial.println(linesHit);  //these three are my test cases
  Serial.println(sensorvalue4);
  Serial.println("-----------------");
  //delay(1000);

  if (sensorvalue4 > 1500 && linesHit == 1 && TimeSinceHitLine > 500) {
    Serial.println("hit the second line, looking for basket based on ir");
    state = BASKET;
  }
  
  TimeSinceHitLine = millis() - TimeHitLine;

  if (prior_state != state) {
    drive(1, 0);
    delay(500);
  }
}

void orient() {
  //state 1, 1the robot has been placed down and has not squared itself to the backbaord. BEHAVIOR: rotates in a left turn.
  //EXIT CASE: both ultrasonics sensors read the same value within a tolerence of 3 cm, can only exit to state 2

  if (state != prior_state) {  //code for entering this state
    prior_state = state;
  }

  if (navpwm > 255){
    navpwm = 255;
  }
  drive(5, navpwm);

  // the below statements should have it such that the left ultrasonic sensor makes its measutment, then the second ultrasonic sensor makes its measurment.
  //yes I know its blocking code, but whats 30 microseconds amongst friends.

  //reading value from left sensor
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * .017;


  //                                              Serial.println("sensor1");
  //                                              Serial.println(duration1);

  //maybe giving the motors the turn right command here as well will help make the 15 microsecond block almost meaningless
  //motor 1 backward
  //motor 3 backward
  //motor 2 forward
  //motor 4 forward

  //reading value from right sensor
  digitalWrite(trigPin2, LOW);  //clear sensor
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * .017;
  //                                          Serial.println("sensor2");
  //                                              Serial.println(duration2);

  deltaDistance = distance1 - distance2;

  if (distance1 < 50 || distance2< 50){
    
    navpwm = 80 + kpnav*abs(deltaDistance);
  
  }
  else{
    navpwm = 255;
  }
  
  Serial.print(navpwm); Serial.print(",");
  Serial.print(distance1); Serial.print(",");
  Serial.print(distance2); Serial.print(",");
  Serial.println(deltaDistance);
  if (abs(duration1-duration2) <= 200 && distance2 < 40 && distance1 < 40) {
    squareCounter = squareCounter + 1;  //the ideas with squarecounter is that the robot wont think its square if a rouge values happens, it will only think its square
                                        //if the ondition for being square is hit 3 times
    if (squareCounter > 3) {
      state = NAVIGATE;
      //Serial.println("made it to state 2");
    }
  }
  else{
    squareCounter = 0;
  }

  if (state != prior_state) {  // clean up for leaving this state
    Serial.println("leaving Orienting state");
    drive(1, 0);  //set all motors forward but not moving
    delay(500); // pause
  }
}

void shoot(){
  if (prior_state != state){
    prior_state = state;
    t0 = millis();
  }

  shootball(); // shoots the ball
  loadball();
  state = NAVIGATE;

  if (state != prior_state){
    Serial.println("leaving shooting state");

  }
}

void shootball(){
  digitalWrite(shooterpin,HIGH);
  if ((millis()-t0) > shooter_duration){
    digitalWrite(shooterpin, LOW); 
  }

}

void loadball(){
  for (int angle = 140; angle >= 0; angle -= 70) {
    myServo.write(angle);  // Set servo angle
    delay(10);  // Short delay for smoother motion
  }
  delay(350);
  

  for (int angle = 0; angle <= 140; angle += 70) {
    myServo.write(angle);  // Set servo angle
    delay(10);  // Short delay for smoother motion
  }
  
  delay(1500);  // Delay for 2 seconds

}
