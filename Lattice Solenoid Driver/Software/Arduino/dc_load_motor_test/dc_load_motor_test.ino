const int EN_PIN = 3;   // EN/IN1: PWM
const int PH_PIN = 5;   // PH/IN2: PWM

const unsigned long RAMP_PERIOD_MS = 8000;
const int MAX_PWM = 255;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(PH_PIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting bidirectional PWM ramp...");
}

void loop() {
  unsigned long time = millis();
  unsigned long t = time % RAMP_PERIOD_MS;

  // Triangle wave from -1.0 to +1.0
  float rampPhase = (float)t / (RAMP_PERIOD_MS / 2);
  float level = (rampPhase <= 1.0) ? rampPhase : (2.0 - rampPhase);
  level = 2.0 * (level - 0.5);

  int pwmValue = (int)(fabs(level) * MAX_PWM);

  if (level >= 0) {
    // Forward: EN = LOW, PH = PWM
    analogWrite(EN_PIN, 0);
    analogWrite(PH_PIN, pwmValue);
  } else {
    // Reverse: EN = PWM, PH = LOW
    analogWrite(EN_PIN, pwmValue);
    analogWrite(PH_PIN, 0);
  }

  Serial.print("Level: ");
  Serial.println(level, 2);

  delay(10);
}
