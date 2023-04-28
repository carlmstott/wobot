#include <Servo.h>

Servo myServo;// Create a servo object
Servo myServo1;
int go;


void setup() {
  Serial.begin(115200);
  myServo.attach(3);  // Attach the servo to pin 9
  myServo1.attach(4);
  int go = 0;
}

void loop() {
  load();
  delay(4000);
}



void load() {
     
       for (int angle = 140; angle >= 0; angle -= 140) {
    myServo.write(angle);  // Set servo angle
 Serial.println(angle);
 
  }
    for (int angle = 0; angle <= 45; angle += 45) {
    myServo1.write(angle);  // Set servo angle

  }
  delay(650);

    for (int angle = 0; angle <= 140; angle += 140) {
    myServo.write(angle);  // Set servo angle
Serial.println("small servo is being sent a signel");
  }
    for (int angle = 45; angle >= 0; angle -= 45) {
    myServo1.write(angle);  // Set servo angle

  }
   delay(2000);
  }
 
