const int stepPin = 6; // STEP connected to pin 6
const int dirPin  = 4; // DIR connected to pin 4

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor (optional)
  Serial.println("DRV8825 Stepper Test Starting...");
}

void loop() {
  // Move forward
  digitalWrite(dirPin, HIGH);
  Serial.println("Moving forward...");
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }

  delay(1000);

  // Move reverse
  digitalWrite(dirPin, LOW);
  Serial.println("Moving reverse...");
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }

  delay(1000);
}
