#include <Wire.h>

// MCP23017 addresses
#define MCP1_ADDR 0x24
#define MCP2_ADDR 0x20

// MCP23017 registers
#define IODIRA 0x01
#define IODIRB 0x00
#define OLATA  0x15
#define OLATB  0x14

// Store pin mappings for each H-bridge: [PHASE, ENABLE]
struct HBridgeControl {
  uint8_t expander;
  uint8_t phase_reg;
  uint8_t phase_bit;
  uint8_t enable_reg;
  uint8_t enable_bit;
};

// Define all 16 H-bridges' control pins
HBridgeControl bridges[16];

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize MCP23017 expanders
  initExpander(MCP1_ADDR);
  initExpander(MCP2_ADDR);

  // Populate bridge mapping
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
  Serial.println("=== I2C Scan ===");
  scanI2C();

  Serial.println("Setting all H-bridges FORWARD");
  setAllBridges(true);
  delay(3000);

  Serial.println("Setting all H-bridges REVERSE");
  setAllBridges(false);
  delay(3000);

  Serial.println("Disabling all H-bridges");
  disableAllBridges();
  delay(3000);
}

void initExpander(uint8_t addr) {
  writeRegister(addr, IODIRA, 0x00); // All A as output
  writeRegister(addr, IODIRB, 0x00); // All B as output
  writeRegister(addr, OLATA, 0x00);  // All A low
  writeRegister(addr, OLATB, 0x00);  // All B low
}

void setAllBridges(bool forward) {
  for (int i = 0; i < 16; i++) {
    setBridge(bridges[i], forward);
  }
}

void disableAllBridges() {
  for (int i = 0; i < 16; i++) {
    setPin(bridges[i].expander, bridges[i].enable_reg, bridges[i].enable_bit, false);
  }
}

void setBridge(HBridgeControl hb, bool forward) {
  setPin(hb.expander, hb.phase_reg, hb.phase_bit, forward);  // PHASE = 1 for forward, 0 for reverse
  setPin(hb.expander, hb.enable_reg, hb.enable_bit, !forward);   // ENABLE = 1 to enable
}

void setPin(uint8_t addr, uint8_t reg, uint8_t bit, bool value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false); // Repeated start

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

void scanI2C() {
  byte error, address;
  int count = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0)
    Serial.println("No I2C devices found.");
}
