#include <TimerOne.h>
#include <PID_v1.h>

// ——— PIN DEFINITIONS ———
const uint8_t EN_PIN      = 3;    // H-bridge EN/IN1 (PWM)
const uint8_t PH_PIN      = 5;    // H-bridge PH/IN2 (PWM)
const uint8_t SENSE_PIN   = A0;   // IPROPI2 → Rsense → analog input

// ——— HARDWARE CONSTANTS ———
const float R_SENSE       = 888.4f;     // Ω sense resistor
const float MIRROR_GAIN   = 1.0f / 1100; // DRV8873 current mirror ratio
const float V_REF         = 5.1f;       // Arduino analog reference (V)
const int   ADC_MAX       = 1023;       // 10-bit ADC
const int   PWM_MAX       = 255*12/18;        // max 8-bit PWM output

// ——— PID PARAMETERS ———
// Tune these for stability & responsiveness
double Kp = 2.0;
double Ki =  1.0;
double Kd =  1.0;

// ——— CONTROL VARIABLES ———
volatile double Input,    // measured sense voltage (V)
                Output,   // PID output → PWM duty [0..PWM_MAX]
                Setpoint; // target sense voltage (V)

// instantiate PID:  DIRECT means Output increases when Input < Setpoint
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// ——— TIMER ISR ———
// Called every CONTROL_DT microseconds
const unsigned long CONTROL_DT_US = 10000; // 10 ms

void pidISR() {
  // 1) Read current-sense voltage
  int raw = analogRead(SENSE_PIN);
  Input = raw * (V_REF / ADC_MAX);

  // 2) Compute new PID output
  myPID.Compute();

  // 3) Drive the H-bridge
  //    We assume Setpoint ≥ 0 → forward; Setpoint < 0 → reverse
  uint8_t duty = constrain(uint8_t(Output), 0, PWM_MAX);
  if (Setpoint >= 0) {
    analogWrite(PH_PIN, duty);
    analogWrite(EN_PIN, 0);
  } else {
    analogWrite(EN_PIN, duty);
    analogWrite(PH_PIN, 0);
  }
}

void setup() {
  // Serial for debug
  Serial.begin(9600);
  while (!Serial);

  // pin modes
  pinMode(EN_PIN, OUTPUT);
  pinMode(PH_PIN, OUTPUT);
  pinMode(SENSE_PIN, INPUT);

  // zero outputs
  analogWrite(EN_PIN, 0);
  analogWrite(PH_PIN, 0);

  // initialize PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, PWM_MAX);
  myPID.SetSampleTime(CONTROL_DT_US / 1000); // in ms

  // start timer ISR
  Timer1.initialize(CONTROL_DT_US);
  Timer1.attachInterrupt(pidISR);

  Serial.println("Async PID force controller ready.");
}

void loop() {
  // ── Example high-level workflow ──
  // 1) Apply load
  //    Compute desired force F (N), then convert to sense voltage:
  //    I_target = F / (1.00062 N/A)
  //    V_set = I_target * R_SENSE * MIRROR_GAIN
  //    Setpoint = V_set;

  double desiredForce = 1.0; // in N
  double I_target     = desiredForce / 1.00062;
  Setpoint            = I_target * R_SENSE * MIRROR_GAIN;

  // simulate doing other tasks for a while
  // any delays or blocking here won't disrupt the PID timing
  runOtherTasks();

  // 3) Remove load
  //    when ready:
  // Setpoint = 0.0;
  
  // for demonstration, we'll loop at 1 Hz
  delay(1000);
}

void runOtherTasks() {
  // placeholder for your other loop-level code —
  // sensors, logging, communications, etc.
  // can block freely and the PID ISR still fires every 10 ms!
}
