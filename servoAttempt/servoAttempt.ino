#include <Servo.h>

Servo myServo;// Create a servo object
Servo myServo1;

int shootPin = 30;


void setup() {
  Serial.begin(115200);
  myServo.attach(3);
  myServo1.attach(4);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(shootPin, OUTPUT);
  digitalWrite(shootPin,LOW);
}

void loop() {

  shoot();
  //delay(500);
  load();
  //delay(500);

}



void load() {
     
    myServo.write(0);  // Set servo angle
 Serial.println(0);
 
    myServo1.write(50);  // Set servo angle

  
  delay(700);
  myServo.write(140);
  myServo1.write(0);

   delay(1000);
  }

void shoot(){
  Serial.println("SHOOOOOOT");
  
  digitalWrite(shootPin, HIGH);
    delay(100);
    digitalWrite(shootPin, LOW);
    
}
 
