
const int multiplexAPin = 17;
const int multiplexBPin = 16;
const int multiplexCPin = 15;
const int multiplexDataPin = 20;

int p0, p1, p2, p3, p4, p5, p6, p7;

void setup() {
  // put your setup code here, to run once:
  pinMode(multiplexAPin, OUTPUT);
  pinMode(multiplexBPin, OUTPUT);
  pinMode(multiplexCPin, OUTPUT);

  Serial.begin(9600); 

  digitalWrite(multiplexAPin, LOW);
  digitalWrite(multiplexBPin, LOW);
  digitalWrite(multiplexCPin, LOW);
}

void loop() {
  // 0
  digitalWrite(multiplexCPin, LOW);
  delayMicroseconds(50);
  p0 = analogRead(multiplexDataPin);

  // 1
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(50);
  p1 = analogRead(multiplexDataPin);

  //  3
  digitalWrite(multiplexBPin, HIGH);
  delayMicroseconds(50);
  p3 = analogRead(multiplexDataPin);
  
  // 2
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(50);
  p2 = analogRead(multiplexDataPin);

  // 6
  digitalWrite(multiplexCPin, HIGH);
  delayMicroseconds(50);
  p6 = analogRead(multiplexDataPin);

  // 7
  digitalWrite(multiplexAPin, HIGH);
  delayMicroseconds(50);
  p7 = analogRead(multiplexDataPin);

  // 5
  digitalWrite(multiplexBPin, LOW);
  delayMicroseconds(50);
  p5 = analogRead(multiplexDataPin);

  // 4
  digitalWrite(multiplexAPin, LOW);
  delayMicroseconds(50);
  p4 = analogRead(multiplexDataPin);

  Serial.print(p0);
  Serial.print(" ");
  Serial.print(p1);
  Serial.print(" ");
  Serial.print(p2);
  Serial.print(" ");
  Serial.print(p3);
  Serial.print(" ");
  Serial.print(p4);
  Serial.print(" ");
  Serial.print(p5);
  Serial.print(" ");
  Serial.print(p6);
  Serial.print(" ");
  Serial.print(p7);
  Serial.print("\n");
}
