const int  EN_PIN           = 3;   // EN/IN1: PWM
const int  PH_PIN           = 5;   // PH/IN2: PWM
const int  MAX_PWM          = 255*4.5/18; // 0–255 full scale

// —— PHASE TIMINGS (all in milliseconds) ——
const unsigned long RAMP_UP_MS    =  500UL;  // time to go 0→100%
const unsigned long DWELL_MS      =  1000UL;  // hold at 100%
const unsigned long RAMP_DOWN_MS  =  500UL;  // time to go 100→0%
const unsigned long REVERSE_MS    =  150UL;  // time to go 0→REVERSE_PERCENT%
const unsigned long WAIT_MS       =  4000UL;  // off time before restarting

// —— REVERSE THROTTLE —— 
const int REVERSE_PERCENT = 50;  // % of MAX_PWM in reverse

// Precompute these
const unsigned long CYCLE_MS = RAMP_UP_MS + DWELL_MS + RAMP_DOWN_MS + REVERSE_MS + WAIT_MS;
const int REVERSE_PWM = 255*4/18;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(PH_PIN, OUTPUT);
  analogWrite(EN_PIN, 0);
  analogWrite(PH_PIN, 0);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting custom PWM sequence...");
}

void loop() {
  unsigned long t = millis() % CYCLE_MS;
  int pwmValue = 0;
  int direction =  1;  // +1 = forward, -1 = reverse, 0 = off

  if (t < RAMP_UP_MS) {
    // Phase 0: ramp 0→100%
    float frac = float(t) / RAMP_UP_MS;
    pwmValue = int(frac * MAX_PWM);
    direction = +1;

  } else if (t < RAMP_UP_MS + DWELL_MS) {
    // Phase 1: dwell at 100%
    pwmValue = MAX_PWM;
    direction = +1;

  } else if (t < RAMP_UP_MS + DWELL_MS + RAMP_DOWN_MS) {
    // Phase 2: ramp 100→0%
    unsigned long dt = t - (RAMP_UP_MS + DWELL_MS);
    float frac = 1.0f - float(dt) / RAMP_DOWN_MS;
    pwmValue = int(frac * MAX_PWM);
    direction = +1;

  } else if (t < RAMP_UP_MS + DWELL_MS + RAMP_DOWN_MS + REVERSE_MS) {
    // Phase 3: ramp 0→REVERSE_PWM (in reverse)
    unsigned long dt = t - (RAMP_UP_MS + DWELL_MS + RAMP_DOWN_MS);
    float frac = float(dt) / REVERSE_MS;
    pwmValue = int(frac * REVERSE_PWM);
    direction = -1;

  } else {
    // Phase 4: wait (off)
    pwmValue = 0;
    direction = 0;
  }

  // Apply outputs
  if (direction > 0) {
    analogWrite(EN_PIN, 0);
    analogWrite(PH_PIN, pwmValue);
  } else if (direction < 0) {
    analogWrite(EN_PIN, pwmValue);
    analogWrite(PH_PIN, 0);
  } else {
    // full off
    analogWrite(EN_PIN, 0);
    analogWrite(PH_PIN, 0);
  }

  // Debug print
  Serial.print("Phase t="); Serial.print(t);
  Serial.print("ms | PWM=");   Serial.print(pwmValue);
  Serial.print(" | Dir=");     Serial.println(direction);

  delay(10);
}
