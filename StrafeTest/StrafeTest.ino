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
int gain; //I will be messing with this outside of the drive function, so I need to decloare it above unlike PWM 


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

void setup() {
  //setup values for state 2, the reason why this is done first is that setupRSLK() declares a bunch of stuff, and a lot of it we want overwritten.
    Serial.begin(115200);

  setupRSLK();
  /* Left button on Launchpad */
  setupWaitBtn(LP_LEFT_BTN);
  /* Red led in rgb led */
  setupLed(RED_LED);
  clearMinMax(sensorMinVal,sensorMaxVal);


}

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

  
}

void loop() {
//before entering our state machine, we need to calibrate our photoresistors.

uint8_t lineColor = DARK_LINE;

  /* Run this setup only once */
  if(isCalibrationComplete == false) {
    floorCalibration();
    isCalibrationComplete = true;
  }
                                                //right now I am testing forward and backward 

//    //psudocode for control system
//    if going left: want a combonation of translating left and rotating around the center axis. 
//    2    1
//    3    4
//    are the wheels. To trsnalate car to the left Wheel direction is:
//    -1    1  which translates to PWM values of: 225    225
//    1     -1                                    225    225
//    
//    To rotate car to the CCW, the wheel directions are:
//    1    -1   meaning that if I want to rotate the car slightly to the left I want to increace the pwm signel going to wheels 2 and 3 and subtract from the pwn value going to wheels 1 and 4                          
//    1    -1
//
//    to rotate the car cockwise, the wheel directions are:
//    -1    1  meaning to rotate right, i add to wheels 1 and 4 while subtracting from wheels 2 and 3. 
//    -1    1
//
//    It seems like I only need a single gain value for my controller, and if I need to rotate left I add to 2/3 and subtract from 1/4.
//    but if I need to rotate to the right, I add to 1/4 and subtract from 2/3.
//
//    the sensor values that I care about are: 1, 3, 5, and 7.
//
//    I think the two values that I am going to be examining for my control system are the value of 3 and 5. it looks like they oscilate between 1000 and 2500.
//
//    My gain is going to be a function of 3 - 5.
//
//    IF WE ARE GOING LEFT:
//          IF 3 - 5 is posative
//              than we need to rotate clockwise, so my gain will be:
//              subtract from wheels 2 and 3 and add to 1 and 4, 
//              but we need to remember that motors 1 and 3 are going forard and motors 2 and 4 are backwards.
//            THEREFORE:
//       we wamt LESS motor 1, LESS motor 2
//               MORE motor 3 and MORE motor 4


//          IF 3 - 5 is negative:
//              THAN we need to rotate CCW:
//            THEREFORE:
//          we want MORE motor 1, MORE motor 2
//          LESS motor 3 and LESS motor 4 

//          OVERALL: to keep us on the line when straifing to the left, we need to ADD our gain to motors 3 and 4 and subtract it from 1 and 2
//
//   IF WE ARE GOING RIGHT: motors 2 and 4 are going forward and motors 1 and 3 are going backwards.
//            IF 3 - 5 is posative:
//                then we need to rotate CCW:
//               we want LESS motor 1 and LESS motor 2
//                we want MORE motor 3 and MORE motor 4
//
//           IF 3 - 5 is negative:
//                   then we need to rotate Clockwise
//                   we need MORE motor 1 and motor 2
//                  we need LESS motor 3 and motor 4

//        OVERALL: to keep us on the line when strafing to the right, we need to ADD gain to motors 3 and 4 and SUBTRACT from motors 1 and 2
//
//  I am tuning my controller such that the base PWM value in will be 175. I will have a gain of between + and - 50.
//  so that means if the sensor values will read 1000 at the lowest and 2500 at the highest, my max delta will be 1500
//  so I need to map -1500 through 1500 onto -50 - 50
//  so my gain will be (3 - 5)/30 = gain, and my base motor PWM inputs will be 175.




//THe below script is used for testing the line sensor 
 gain = (sensorVal[3]-sensorVal[5])/30
 if gain+pwm >225{
  gain = 50; //this if statement prevents me from sending my motors a pwm value of over 225
 }

 //linefollow left
 drive(2, 175, gain);


 //linefollow right, commented out for now, will test later
  //drive(3, 175, gain); //175 is base pwm signel
  
}



void strafe(int i,int pwm, gain){ // drive(1) means drive forward
  switch (i){
    case 1: // drive forward
      set_direction(1,1);
      set_direction(2,1);
      set_direction(3,1);
      set_direction(4,1);

    analogWrite(motor1pwmpin,pwm);
    analogWrite(motor2pwmpin,pwm);
    analogWrite(motor3pwmpin,pwm);
    analogWrite(motor4pwmpin,pwm);
      break;
      
    case 2: // strafe left
      set_direction(1,1);
      set_direction(2,-1);
      set_direction(3,1);
      set_direction(4,-1);
      
/*  COMMENTED OUT FOR TESTING, WILL COMMENT BACK IN WHEN WE ARE READY FOR IT TO LINEFOLLOW
    analogWrite(motor1pwmpin,pwm-gain);
    analogWrite(motor2pwmpin,pwm-gain);
    analogWrite(motor3pwmpin,pwm+gain);
    analogWrite(motor4pwmpin,pwm+gain);
*/
                                                      //printstatements for testing
                                                      Serial.println("wheels on the front side PWM input:");
                                                      Serial.print(pwm-gain);
                                                      Serial.println("wheels on the back side PWM input");
                                                      Serial.print(pwm+gain);
      break;
      
    case 3: // strafe right
      set_direction(1,-1);
      set_direction(2,1);
      set_direction(3,-1);
      set_direction(4,1);

    analogWrite(motor1pwmpin,pwm-gain);
    analogWrite(motor2pwmpin,pwm-gain);
    analogWrite(motor3pwmpin,pwm+gain);
    analogWrite(motor4pwmpin,pwm+gain);
      break;
      
    case 4: // reverse
      set_direction(1,-1);
      set_direction(2,-1);
      set_direction(3,-1);
      set_direction(4,-1);

          analogWrite(motor1pwmpin,pwm);
    analogWrite(motor2pwmpin,pwm);
    analogWrite(motor3pwmpin,pwm);
    analogWrite(motor4pwmpin,pwm);
      break;
      
    case 5: //rotate in place to the left 
    set_direction(1,-1);
    set_direction(2,1);
    set_direction(3,1);
    set_direction(4,-1);

        analogWrite(motor1pwmpin,pwm);
    analogWrite(motor2pwmpin,pwm);
    analogWrite(motor3pwmpin,pwm);
    analogWrite(motor4pwmpin,pwm);
    
  }



  
    
  
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
