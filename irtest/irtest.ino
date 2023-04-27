const int irpinl = 5;
const int irpinm = 6;
const int irpinr = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(irpinl,INPUT);
  pinMode(irpinm,INPUT);
  pinMode(irpinr,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
  Serial.println(digitalRead(irpinl));Serial.println(digitalRead(irpinm));Serial.println(digitalRead(irpinr));
  Serial.println("-------------");

}
