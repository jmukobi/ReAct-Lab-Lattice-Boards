#include <Wire.h>

// MCP23017 I2C addresses
#define MCP1_ADDR 0x26
#define MCP2_ADDR 0x24

// MCP23017 register addresses
#define IODIRA 0x01
#define IODIRB 0x00
#define OLATA  0x15
#define OLATB  0x14

struct HBridgeControl {
  uint8_t expander;
  uint8_t phase_reg;
  uint8_t phase_bit;
  uint8_t enable_reg;
  uint8_t enable_bit;
};

HBridgeControl bridges[16];

void setup() {
  Serial.begin(115200);
  Wire.begin();

  initExpander(MCP1_ADDR);
  initExpander(MCP2_ADDR);

  for (int i = 0; i < 8; i++) {
    bridges[i] = {
      MCP1_ADDR,
      (i < 4) ? OLATB : OLATA,
      (uint8_t)(i * 2 % 8),
      (i < 4) ? OLATB : OLATA,
      (uint8_t)(i * 2 % 8 + 1)
    };
  }
  for (int i = 8; i < 16; i++) {
    bridges[i] = {
      MCP2_ADDR,
      (i - 8 < 4) ? OLATB : OLATA,
      (uint8_t)((i - 8) * 2 % 8),
      (i - 8 < 4) ? OLATB : OLATA,
      (uint8_t)((i - 8) * 2 % 8 + 1)
    };
  }
}

void loop() {
  Serial.println("FORWARD (ramp)");
  for (int i = 0; i < 16; i++) {
    rampSolenoid(bridges[i]);
  }

  delay(1000); // Pause before reverse

  Serial.println("REVERSE (30%)");
  for (int i = 0; i < 16; i++) {
    reversePulse(bridges[i]);
  }

  delay(1000); // Pause before repeat
}

// === ACTUATION FUNCTIONS ===

void rampSolenoid(HBridgeControl hb) {
  // Set PH = 1 (forward)
  setPin(hb.expander, hb.phase_reg, hb.phase_bit, true);

  //print status
  Serial.print("Ramping solenoid on expander ");
  // RAMP UP: 10 steps over 200ms
  for (int duty = 0; duty <= 100; duty += 5) {
    pwmBurst(hb, 20, duty);  // 20ms per step
  }

  Serial.println("Hold at 100% for 100ms");
  // HOLD at 100% for 100ms
  pwmBurst(hb, 100, 100);

  Serial.println("Ramping down");
  // RAMP DOWN
  for (int duty = 100; duty >= 0; duty -= 5) {
    pwmBurst(hb, 20, duty);
  }
  Serial.println("Hold at 0% for 100ms");
  disableBridge(hb);
}

void reversePulse(HBridgeControl hb) {
  // Set PH = 0 (reverse)
  setPin(hb.expander, hb.phase_reg, hb.phase_bit, false);

  // Run at 30% for 100ms
  pwmBurst(hb, 100, 30);

  disableBridge(hb);
}

// Simulate PWM for duration (ms) at given duty (%)
void pwmBurst(HBridgeControl hb, int duration_ms, int duty_percent) {
  const int freq = 500; // ~500 Hz PWM
  const int period_us = 1000000 / freq; // 2000us
  int high_time = period_us * duty_percent / 100;
  int low_time = period_us - high_time;
  int cycles = (duration_ms * 1000L) / period_us;

  for (int i = 0; i < cycles; i++) {
    setPin(hb.expander, hb.enable_reg, hb.enable_bit, false); // EN = 0 → ON
    delayMicroseconds(high_time);
    setPin(hb.expander, hb.enable_reg, hb.enable_bit, true);  // EN = 1 → OFF
    delayMicroseconds(low_time);
  }
}

void disableBridge(HBridgeControl hb) {
  setPin(hb.expander, hb.phase_reg, hb.phase_bit, false);
  setPin(hb.expander, hb.enable_reg, hb.enable_bit, false); // Both LOW = Hi-Z
}

// === MCP23017 I/O Helpers ===

void initExpander(uint8_t addr) {
  writeRegister(addr, IODIRA, 0x00);
  writeRegister(addr, IODIRB, 0x00);
  writeRegister(addr, OLATA, 0x00);
  writeRegister(addr, OLATB, 0x00);
}

void setPin(uint8_t addr, uint8_t reg, uint8_t bit, bool value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)1);
  uint8_t current = Wire.read();
  uint8_t updated = value ? (current | (1 << bit)) : (current & ~(1 << bit));
  writeRegister(addr, reg, updated);
}

void writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}
