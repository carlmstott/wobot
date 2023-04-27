int trigPin1 = 24;
int trigPin2 = 29;
int echoPin1 = 23;
int echoPin2 = 28;
long duration1;
long distance1;
long duration2;
long distance2;
long deltaDistance;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly: 
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * .017;


  //reading value from right sensor
  digitalWrite(trigPin2, LOW);  //clear sensor
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * .017;
  int ddistance = abs(distance1-distance2);
  Serial.print(duration1); Serial.print(",");Serial.println(duration2);
}
