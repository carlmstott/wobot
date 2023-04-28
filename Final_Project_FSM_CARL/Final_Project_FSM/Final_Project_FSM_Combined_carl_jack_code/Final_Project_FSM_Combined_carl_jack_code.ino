
#include <Servo.h>
Servo feederServo;
const int motorbias = 0;

const int max_speed_pwm = 200; // cap sinceo our motors are really fast and powerful




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
double gain; //I will be messing with this outside of the drive function, so I need to decloare it above unlike PWM 
double intergral; //adding intergral control
int pwm=125;
int input; //combination of intergral and proportonal gain


bool isScanned = false;
const int buffersize = 25;
const int strafe_pwm = 100;
bool isSquare = false; // for ir sensing purposes


//variables declared for state 1
int trigPin1=24;
int trigPin2=29;
int echoPin1=23;
int echoPin2=28;
long duration1;
long distance1;
long duration2;
long distance2;
long deltaDistance;
unsigned long startTime;
unsigned long currentTime;
int squareCounter=0;




//variables and declarations for state 2 RSLK library webpage https://fcooper.github.io/Energia-RSLK-Library/_simple_r_s_l_k_8h.html#afa1b08c477adccb29dc3ad97a58b63c7
#include "SimpleRSLK.h"
bool isCalibrationComplete = false;
uint16_t sensorVal[LS_NUM_SENSORS]; //LS_NUM_SENSORS is the value 8
uint16_t sensorCalVal[LS_NUM_SENSORS];
uint16_t sensorMaxVal[LS_NUM_SENSORS];
uint16_t sensorMinVal[LS_NUM_SENSORS];
int TimeHitLine = 0;
int  TimeSinceHitLine=0;
int linesHit=0;


//below lines are declarations jack uses for the pwm control system for squaring against the back wall
int navpwm, t0;
float kpnav = .08;
const float kpline = .005; //from .1-.5
const float kiline = 0.0005; //from 0-.0005
int cumddistance;
int counter;
float distance1_buffer, distance2_buffer;


//declarations for the shooter
const int shooterpin = 30;
const int shooter_duration = 200; // milliseconds of how long to keep the switch open
const int servopin = 9;
int shootPin=30;


int IRbeaconLeft =5;
int IRbeaconMiddle =6;
int IRbeaconRight =8;
int direction;




void setup() {

  //setup values for state 2, the reason why this is done first is that setupRSLK() declares a bunch of stuff, and a lot of it we want overwritten.
    Serial.begin(115200);

  setupRSLK();
  /* Left button on Launchpad */
  setupWaitBtn(LP_RIGHT_BTN);
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



//shooter and feeder pin declarations, written by jack
  pinMode(shooterpin, OUTPUT);
  digitalWrite(shooterpin, LOW);
feederServo.attach(servopin);

}


/////////////////////////////////following 2 functions are used to calibrate the photoresistors///////////////////////
void floorCalibration() {
  /* Place Robot On Floor (no line) */
  delay(2000);
  String btnMsg = "Push left button on Launchpad to begin calibration.\n";
  btnMsg += "Make sure the robot is on the floor away from the line.\n";
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_RIGHT_BTN,btnMsg,RED_LED);

  delay(1000);

  Serial.println("Running calibration on floor");
  simpleCalibrate();
  Serial.println("Reading floor values complete");

  btnMsg = "Push left button on Launchpad to begin line following.\n";
  btnMsg += "Make sure the robot is on the line.\n";
  /* Wait until button is pressed to start robot */
  waitBtnPressed(LP_RIGHT_BTN,btnMsg,RED_LED);
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



//state 4, robot is centered and in front of active hoop. Robot behavior: robot goes through loading and firing cycle. 
                  //EXIT CASE: any of the side IR sensors return 1 and middle IR sensor returns zero. Exits to state 3

switch (state) {
  
case 1: { //state 1, 1the robot has been placed down and has not squared itself to the backbaord. BEHAVIOR: rotates in a left turn. 
                    //EXIT CASE: both ultrasonics sensors read the same value within a tolerence of 3 cm, can only exit to state 2
  
     //drive(5,200);

    // the below statements should have it such that the left ultrasonic sensor makes its measutment, then the second ultrasonic sensor makes its measurment.
    //this is blocking code but it blocks for so little time that it makes no difference.

    //reading value from left sensor
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin1, LOW);
    duration1=pulseIn(echoPin1, HIGH);
    distance1=duration1*.017;
  

//                                              Serial.println("sensor1");
//                                              Serial.println(duration1);
                                              

   //reading value from right sensor
    digitalWrite(trigPin2, LOW);  //clear sensor
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin2, LOW);
    duration2=pulseIn(echoPin2, HIGH);
    distance2=duration2*.017;
//                                          Serial.println("sensor2");
//                                              Serial.println(duration2);
                                                          
      
      deltaDistance = duration1 - duration2; //using durations to calulate deltaDistance instead of distance values becasue duration has a higher resolution.

//jack's P control for squaring against the wall BEING TESTED
  if (distance1 < 40 || distance2< 40){

    navpwm = 65 + kpnav*abs(distance1-distance2);
    

drive(5,navpwm);
  }else{
    drive(5,200); //THIS IS THE DEFAULT VALUE FOR DRIVE
  }
  //ABOVE BEING TESTED
  
      
      if (abs(deltaDistance) <= 200 && distance2 <50 && distance1 <50) {
        squareCounter=squareCounter+1; //the ideas with squarecounter is that the robot wont think its square if a rouge values happens, it will only think its square
                                        //if the ondition for being square is hit 3 times
        if (squareCounter > 2){
          Serial.println("made it to state 2");
          drive(1,0);
          delay(500);
         //delay .5 seconds, this helps with visual conformation that we have made it to the next state.
        state = 2;
       
       
        }
      }
    }
break;

    
    ///////////////////////////////////////////state 1 end/////////////////////////////////////////////////////////////////////////


case 2:
{
    //state 2, robot has squared itself to the backbaord. BEHAVIOR: robot translates forward. 
                  //EXIT CASE: HasHitLine=2, meaning robot is on closest line to hoops, can only exit to state 3
                  //NOTE: line hit is defined as both middle photoresitors returning black
                  

//Get sensor array data and compute position of line
  readLineSensor(sensorVal);
  readCalLineSensor(sensorVal,
            sensorCalVal,
            sensorMinVal,
            sensorMaxVal,
            lineColor); //this code updates the sensors min and max values based on the current data. SOMETHING TO TEST??
            
  uint32_t linePos = getLinePosition(sensorCalVal,lineColor); //linePos returns between 0 and 7000. 1000 when line is detected under forwardmost sensor and 7000 when detected under aftmost sensor 


                                   
  

if (linesHit == 0){
  drive(1,200); //go straight, we want to go fast.
}

if (linesHit == 1) { //jacks control system activates after we have hit the first line
//this reads ultrasonic sensors for control system that keeps us squared to the back wall
//while we translate forward, written by jack
digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = duration1 * .017;
    digitalWrite(trigPin2, LOW);  //clear sensor
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = duration2 * .017;

    //the below back wall squaring control system made by Jack Quao
    int ddistance = duration1-duration2;
    int threshold = 150; //changed from 150-300-150
    // duration 2500 min
    cumddistance = cumddistance + ddistance;
    if (cumddistance > 10000000){
      cumddistance = 10000000;
    }
    int pwmfb = abs(ddistance)*kpline + kiline*abs(cumddistance);
    int pwm = pwmfb + 80;
    if (pwm > 255){
      pwm = 255;
    }
    if (ddistance > threshold){
      driveoneside(1,pwm);
      driveoneside(-1,pwm*2/3);
    }
    else if (ddistance < -threshold){
      driveoneside(-1,pwm);
      driveoneside(1,pwm*2/3);
    }
    else{
      if (distance1 < 85) {
      drive(1,max_speed_pwm/2);
      }else{
        drive(1,85);
      }
      }
    Serial.print("pwmfb:");
    Serial.println(pwmfb);
    Serial.print("distance1:");
    Serial.println(distance1);
    Serial.print("distance2:");
    Serial.println(distance2);

}







//below 2 if statements check to see when the forwardmost sensor (sensor 7)
//has hit the first line, and then the second line
if (sensorVal[6] > 2000 && linesHit==0) {//sensor 7 is the one in the front
  //Serial.println("hit the first line");
  TimeHitLine=millis();
 linesHit=1;
 TimeSinceHitLine=millis()-TimeHitLine;
}

if(sensorVal[6] > 2000 && linesHit==1 && TimeSinceHitLine > 500){
//if(sensorVal[4] > 1500 && linesHit==1 && TimeSinceHitLine > 800){
  Serial.println("hit the second line, entering state 3");
  drive(1,0);
  delay(1000); //we stop for a second to make it clear we are entering another state and to dispell inertia
  state = 3;
  
  
}
TimeSinceHitLine=millis()-TimeHitLine; //this counts to make sure that I dont register hitting the second line until .8 seconds have passed
                                       //this makes sure I dont count the same line as being hit twice.


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

 readLineSensor(sensorVal); // we need to read line sensor data for this

 readCalLineSensor(sensorVal, //dont know if I need to call either of these two
            sensorCalVal,
            sensorMinVal,
            sensorMaxVal,
            lineColor);


  uint32_t linePos = getLinePosition(sensorCalVal,lineColor);

 gain =((sensorVal[6]-932)-(sensorVal[0]-1147)*1.11)/26 - 7; //need to map -1300 <-> 1300 onto -50 <-> 50 gain is the same as error


 intergral = intergral + gain;
 //Serial.println(intergral);
 //Serial.println(gain);


 input=gain+.1*intergral;
 
  if (intergral >50){
    intergral=50;
 }
  if (intergral <-50){
    intergral = -50;
 }


if (input+pwm > 225) {
    input = 225; //this if statement prevents me from sending my motors a pwm value of over 225
}
if (input-pwm<-255){
    input=-pwm;

}
     //Serial.println(input+pwm);




if (isLeft==0){
  //NOTE that these conditions are met when they read a signel of zero becasue we use a pullup resistor circuit for the IR sensors.
  strafe(1, pwm, input);
 
  direction=1;
}

 if (isRight==0){
  strafe(2, pwm, input);

  direction=2;

}

if (isMiddle==0){ //meaning the IR sensor in the middle is seeing a beacon, meaning we are VERY CLOSE to a cross
if (direction==1){ //this means the most recent direction that was sensed was left. 
  //want to drive, NOT STRAIF, to the left. we dont want our control system to freak out when linePos shoots up to 7000
 strafe(1, pwm, input);
  
}
if (direction==2){ //this means the most recent direction sensed was right.
  //want to drive, NOT STRAIF, to the right. we dont want our control system to freak out when  linePos shoots up to 7000
  strafe(2, pwm, input);
 
}


 if (sensorVal[0]>1500 && sensorVal[1]>1500 && sensorVal[2]>1500 && sensorVal[3]>1500 && sensorVal[4]>1500 && sensorVal[5]>1500 && sensorVal[6]>1500 && sensorVal[7] >1500){
 //if thise condition is satisfied, we are right on top of a cross.

drive(1,0); //we want to stop driving when we change state for visual conformation and to dissepate inertia.
delay(500); //taking a .5 second pause in order to visually verify we are making it to the next state
 state=4;
}

} //end of the middle IR sensor check statement


}
break;

////////////////////////////////////////////////END OF STATE 3 /////////////////////////////////////////////////////////////////////////

case 4:
{ 
  //once we reach case 4, all of our photoresistors return above 2000, meaning we are above a cross.
 //ALSO: our front IR sensor has returned true, meaning that the cross we are on is infront of the beacon that is on.
 // THIS MEANS it is time to shoot. Tyler has not written the shoot function yet, so that will go here

int isLeft = digitalRead(IRbeaconLeft);
int isMiddle = digitalRead(IRbeaconMiddle);
int isRight = digitalRead(IRbeaconRight);

 shoot();
 load();


 
 if(isMiddle == 1){ //meaning the IR beacon in front of us is no longer on, meaning that we need to go back to state 3
  state = 3;
  
 }
}

/////////////////////////////////////////////////////////// CASE 4 END ///////////////////////////////////////////////////////////

} //state machine ends here
}  //loop ends here




//this function was written by jack, is used to center the robot while it translates forward in state 2
void driveoneside(int i, int pwm){//-1 is left and 1 is right side
  if (i > 0){
    set_direction(1,1);
    set_direction(4,1);
    set_direction(2,-1);
    set_direction(3,-1);
  }
  else{
    set_direction(1,-1);
    set_direction(4,-1);
    set_direction(2,1);
    set_direction(3,1);
  }
  analogWrite(motor1pwmpin, pwm);
  analogWrite(motor2pwmpin, pwm);
  analogWrite(motor3pwmpin, pwm);
  analogWrite(motor4pwmpin, pwm);
}



void strafe(int i,int pwm, int input){
  switch (i){
      
    case 1: // strafe left
      set_direction(1,1);
      set_direction(2,-1);
      set_direction(3,1);
      set_direction(4,-1);
      

    analogWrite(motor1pwmpin,pwm+input);
    analogWrite(motor2pwmpin,pwm-input);
    analogWrite(motor3pwmpin,pwm+input);
    analogWrite(motor4pwmpin,pwm-input);
                                          
                                                      
      break;
      
      
    case 2: // strafe right
      set_direction(1,-1);
      set_direction(2,1);
      set_direction(3,-1);
      set_direction(4,1);

 
    analogWrite(motor1pwmpin,pwm-input);
    analogWrite(motor2pwmpin,pwm+input);
    analogWrite(motor3pwmpin,pwm-input);
    analogWrite(motor4pwmpin,pwm+input);
    

      break;
    
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
    case 5: //rotate in place to the left 
    set_direction(1,-1);
    set_direction(2,1);
    set_direction(3,1);
    set_direction(4,-1);
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

void shoot(){
  Serial.println("SHOOOOOOT");
  
  digitalWrite(shootPin, HIGH);
    delay(20);
    digitalWrite(shootPin, LOW);
}

void load(){
  Serial.println("LOOOOAAADDDDDD");
}
