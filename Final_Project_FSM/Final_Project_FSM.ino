
int state = 1; //we start in state 1
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
int trigPin1=24;
int trigPin2=26;
int echoPin1=23;
int echoPin2=25;
long duration1;
long distance1;
long duration2;
long distance2;
long deltaDistance;
unsigned long startTime;
unsigned long currentTime;


//variables and declarations for state 2 RSLK library webpage https://fcooper.github.io/Energia-RSLK-Library/_simple_r_s_l_k_8h.html#afa1b08c477adccb29dc3ad97a58b63c7
#include "SimpleRSLK.h"
bool isCalibrationComplete = false;
uint16_t sensorVal[LS_NUM_SENSORS]; //LS_NUM_SENSORS is the value 8
uint16_t sensorCalVal[LS_NUM_SENSORS];
uint16_t sensorMaxVal[LS_NUM_SENSORS];
uint16_t sensorMinVal[LS_NUM_SENSORS];
int HasHitLine = 0; 



//variables and declarations for state 3
//pins
int IRbeaconLeft;
int IRbeaconMiddle;
int IRbeaconRight;
int direction;
//variables used for 


void setup() {

  //setup values for state 2, the reason why this is done first is that setupRSLK() declares a bunch of stuff, and a lot of it we want overwritten.
    Serial.begin(115200);

  setupRSLK();
  /* Left button on Launchpad */
  setupWaitBtn(LP_LEFT_BTN);
  /* Red led in rgb led */
  setupLed(RED_LED);
  clearMinMax(sensorMinVal,sensorMaxVal);

  

  
  //motor designations: motor 1 is front left, motor 2 is front right, motor 3 is bottom left and motor 4 is bottom right.


  //state 1 pins
  //Ultrasonic sensor designations: sensor 1 is left, sensor 2 is right.
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
startTime=micros(); //Timer used for ultrasonic sensors






//state 3 pins and setup
 pinMode(IRbeaconLeft, INPUT);
 pinMode(IRbeaconMiddle, INPUT); //in the demo these are pullups
 pinMode(IRbeaconRight, INPUT);



}


/////////////////////////////////following 2 functions are used to calibrate the photoresistors///////////////////////
void floorCalibration() {
  /* Place Robot On Floor (no line) */
  delay(2000);
  String btnMsg = "Push left button on Launchpad to begin calibration.\n";
  btnMsg += "Make sure the robot is on the floor away from the line.\n";
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_LEFT_BTN,btnMsg,RED_LED);

  delay(1000);

  Serial.println("Running calibration on floor");
  simpleCalibrate();
  Serial.println("Reading floor values complete");

  btnMsg = "Push left button on Launchpad to begin line following.\n";
  btnMsg += "Make sure the robot is on the line.\n";
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_LEFT_BTN,btnMsg,RED_LED);
  delay(1000);

  //enableMotor(BOTH_MOTORS); leftover from RSLK code, dont think it has a use.
}

void simpleCalibrate() {
  
  //in the rslk code, motors run foward for a second here, I dont know if we need to do that.

  for(int x = 0;x<100;x++){
    readLineSensor(sensorVal);
    setSensorMinMax(sensorVal,sensorMinVal,sensorMaxVal);
  }

  // in RSLK, all motors are disabled here, IDK if we need to do that.
}

/////////////////////////////////above 2 functions are used to calibrate the photoresistors///////////////////////



void loop() {

//before entering our state machine, we need to calibrate our photoresistors.

uint8_t lineColor = DARK_LINE;

  /* Run this setup only once */
  if(isCalibrationComplete == false) {
    floorCalibration();
    isCalibrationComplete = true;
  }

                       
//state 3, robot has reached close line. BEHAVIOR: robot moves in the direction of most recently active active side IR sensor. 
                  //EXIT CASE: both side IR sensors return 0, front IR sensor returns 1, and all photoresistors return 1. Exits to state 4

//state 4, robot is centered and in front of active hoop. Robot behavior: robot goes through loading and firing cycle. 
                  //EXIT CASE: any of the side IR sensors return 1 and middle IR sensor returns zero. Exits to state 3

switch (state) {
  
case 1: { //state 1, 1the robot has been placed down and has not squared itself to the backbaord. BEHAVIOR: rotates in a left turn. 
                    //EXIT CASE: both ultrasonics sensors read the same value within a tolerence of 3 cm, can only exit to state 2
  
  //motor 1 backward
  //motor 3 backward
  //motor 2 forward
  //motor 4 forward


//reading value from left sensor
    //this sequence happens over 12 microseconds, for the firt 2, trigPin is high. for the last 10, trigPin is low
    currentTime=micros();
     digitalWrite(trigPin1, LOW);
     digitalWrite(trigPin2, LOW);
     

    // the below statements should have it such that the left ultrasonic sensor makes its measutment, then the second ultrasonic sensor makes its measurment, then their measurments are comapred 
    if (currentTime-startTime > 2) {
      digitalWrite(trigPin1, HIGH);
    }
    
    if (currentTime-startTime == 12) { //at this point, the trigger should have been on for 10 microseconds
      digitalWrite(trigPin1, LOW);
      duration1= pulseIn(echoPin1, HIGH);
      distance1= duration1 * .034/2; //https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/, should convert distance into CM

    //print statement here for distance1 maybe
    }
      //calculating distance meaurment for sensor 2
    if (currentTime-startTime > 14){
      digitalWrite(trigPin2, HIGH);
    }

    if (currentTime-startTime == 24){
      digitalWrite(trigPin2, LOW);
      duration2= pulseIn(echoPin2, HIGH);
      distance2= duration2 * .034/2;
      
      deltaDistance = distance1 - distance2;
      if (abs(deltaDistance) <= 3) {
        state = 2;
      }
      startTime=micros(); //resetting start time after deltaDistance has been calculated
    }
break;
}
    
    ///////////////////////////////////////////state 1 end/////////////////////////////////////////////////////////////////////////


case 2:
{
    //state 2, robot has squared itself to the backbaord. BEHAVIOR: robot translates forward. 
                  //EXIT CASE: HasHitLine=2, meaning robot is on closest line to hoops, can only exit to state 3
                  //NOTE: line hit is defined as both middle photoresitors returning black
                  
  //motor 1 forward
  //motor 3 forward
  //motor 2 forward
  //motor 4 forward

//Get sensor array data and compute position of line
  readLineSensor(sensorVal);
  readCalLineSensor(sensorVal,
            sensorCalVal,
            sensorMinVal,
            sensorMaxVal,
            lineColor); //this code updates the sensors min and max values based on the current data. SOMETHING TO TEST??
  uint32_t linePos = getLinePosition(sensorCalVal,lineColor);
  
 if (sensorVal[3] && sensorVal[4] >2000)
  HasHitLine=HasHitLine+1; //incrementing how many lines the robot has hit
     
 }

 if (HasHitLine = 2){
  state = 3;
 }
 
break;

   ///////////////////////////////////////////state 2 end/////////////////////////////////////////////////////////////////////////


case 3:
{
//state 3, robot has reached close line. BEHAVIOR: robot moves in the direction of most recently active active side IR sensor. 
                  //EXIT CASE: both side IR sensors return 0, front IR sensor returns 1, and all photoresistors return 1. Exits to state 4

// the point of the direction variable is to store the direction that the car was moving most recently. This comes into effect when the middle IR sensor is the only one sensing something
//but we still need to go in a direction.

int isLeft = digitalRead(IRbeaconLeft);
int isMiddle = digitalRead(IRbeaconMiddle);
int isRight = digitalRead(IRbeaconRight);


if (isLeft=1){
  
  //motor 1 backward
  //motor 4 backward
  //motor 2 forward
  //motor 3 forward
  direction=1;
}

 if (isRight=1){

  //motor 1 forward
  //motor 4 forward
  //motor 2 backward
  //motor 3 backward
  direction=2;

}

if (isMiddle=1){
if (direction==1){ //this means the most recent direction that was sensed was left. 
  //motor 1 backward
  //motor 4 backward
  //motor 2 forward
  //motor 3 forward 
}
if (direction==2){ //this means the most recent direction sensed was right.
  //motor 1 forward
  //motor 4 forward
  //motor 2 backward
  //motor 3 backward 
}

  readLineSensor(sensorVal);
  
  readCalLineSensor(sensorVal,
            sensorCalVal,
            sensorMinVal,
            sensorMaxVal,
            lineColor);

 if (sensorVal[0]>2000 && sensorVal[1]>2000 && sensorVal[2]>2000 && sensorVal[3]>2000 && sensorVal[4]>2000 && sensorVal[5]>2000 && sensorVal[6]>2000 && sensorVal[7] >2000){
 //if thise condition is satisfied, we are right on top of a cross.

 state=4;
  
}

break;
}
}
}
}






void drive(int i,int pwm){ // drive(1) means drive forward
  switch (i){
    case 1: // drive forward
      set_direction(1,1);
      set_direction(2,1);
      set_direction(3,1);
      set_direction(4,1);
      
      break;
    case 2: // strafe left
      set_direction(1,1);
      set_direction(2,-1);
      set_direction(3,1);
      set_direction(4,-1);
      break;
    case 3: // strafe right
      set_direction(1,-1);
      set_direction(2,1);
      set_direction(3,-1);
      set_direction(4,1);
      break;
    case 4: // reverse
      set_direction(1,-1);
      set_direction(2,-1);
      set_direction(3,-1);
      set_direction(4,-1);
      break;
  }

  analogWrite(motor1pwmpin,pwm);
  analogWrite(motor2pwmpin,pwm);
  analogWrite(motor3pwmpin,pwm);
  analogWrite(motor4pwmpin,pwm);
    
  
}


void set_direction(int i,int k){
  switch (i) {
    case 1:
      if (k > 0){
        digitalWrite(motor1dir1pin,HIGH);
        digitalWrite(motor1dir2pin,LOW);
      }
      else if (k<0){
        digitalWrite(motor1dir1pin,LOW);
        digitalWrite(motor1dir2pin,HIGH);
      }
      else{
        digitalWrite(motor1dir1pin,HIGH);
        digitalWrite(motor1dir2pin,HIGH);
      }
      break;

    case 2:
      if (k > 0){
        digitalWrite(motor2dir1pin,HIGH);
        digitalWrite(motor2dir2pin,LOW);
      }
      else if (k<0){
        digitalWrite(motor2dir1pin,LOW);
        digitalWrite(motor2dir2pin,HIGH);
      }
      else{
        digitalWrite(motor2dir1pin,HIGH);
        digitalWrite(motor2dir2pin,HIGH);
      }
      break;
    case 3:
      if (k > 0){
        digitalWrite(motor3dir1pin,HIGH);
        digitalWrite(motor3dir2pin,LOW);
      }
      else if (k<0){
        digitalWrite(motor3dir1pin,LOW);
        digitalWrite(motor3dir2pin,HIGH);
      }
      else{
        digitalWrite(motor3dir1pin,HIGH);
        digitalWrite(motor3dir2pin,HIGH);
      }
      break;
    case 4:
      if (k > 0){
        digitalWrite(motor4dir1pin,HIGH);
        digitalWrite(motor4dir2pin,LOW);
      }
      else if (k<0){
        digitalWrite(motor4dir1pin,LOW);
        digitalWrite(motor4dir2pin,HIGH);
      }
      else{
        digitalWrite(motor4dir1pin,HIGH);
        digitalWrite(motor4dir2pin,HIGH);
      }
      break;

    
  }


  
}
