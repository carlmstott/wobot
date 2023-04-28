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

void setup() {
  //state 1 pins
  //Ultrasonic sensor designations: sensor 1 is left, sensor 2 is right.
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
startTime=micros(); //Timer used for ultrasonic sensors

}

void loop() {
  

  
}
