#include <Wire.h>

// MCP23017 I2C addresses
#define MCP1_ADDR 0x26
#define MCP2_ADDR 0x24

// MCP23017 register addresses
#define IODIRA 0x00
#define IODIRB 0x01
#define OLATA  0x14
#define OLATB  0x15

// Struct to define each solenoid's PHASE and ENABLE pins
struct HBridgeControl {
  uint8_t expander;
  uint8_t phase_reg;
  uint8_t phase_bit;
  uint8_t enable_reg;
  uint8_t enable_bit;
};

// Define the mapping for each solenoid
HBridgeControl bridges[16] = {
  { MCP1_ADDR, OLATB, 0, OLATB, 1 }, // PH1, EN1
  { MCP1_ADDR, OLATB, 2, OLATB, 3 }, // PH2, EN2
  { MCP1_ADDR, OLATB, 4, OLATB, 5 }, // PH3, EN3
  { MCP1_ADDR, OLATB, 6, OLATB, 7 }, // PH4, EN4
  { MCP1_ADDR, OLATA, 0, OLATA, 1 }, // PH5, EN5
  { MCP1_ADDR, OLATA, 2, OLATA, 3 }, // PH6, EN6
  { MCP1_ADDR, OLATA, 4, OLATA, 5 }, // PH7, EN7
  { MCP1_ADDR, OLATA, 6, OLATA, 7 }, // PH8, EN8
  { MCP2_ADDR, OLATB, 0, OLATB, 1 }, // PH9, EN9
  { MCP2_ADDR, OLATB, 2, OLATB, 3 }, // PH10, EN10
  { MCP2_ADDR, OLATB, 4, OLATB, 5 }, // PH11, EN11
  { MCP2_ADDR, OLATB, 6, OLATB, 7 }, // PH12, EN12
  { MCP2_ADDR, OLATA, 0, OLATA, 1 }, // PH13, EN13
  { MCP2_ADDR, OLATA, 2, OLATA, 3 }, // PH14, EN14
  { MCP2_ADDR, OLATA, 4, OLATA, 5 }, // PH15, EN15
  { MCP2_ADDR, OLATA, 6, OLATA, 7 }  // PH16, EN16
};

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize both expanders
  initExpander(MCP1_ADDR);
  initExpander(MCP2_ADDR);
}

void loop() {
  Serial.println("H-Bridge #10 FORWARD");
  setBridge(bridges[9], "forward"); // H-Bridge #10 corresponds to index 9
  delay(1); // 0.001s ON

  Serial.println("H-Bridge #9 OFF");
  setBridge(bridges[9], "off");
  delay(50);
  // set # 11 forward
  Serial.println("H-Bridge #11 FORWARD");
  setBridge(bridges[10], "forward"); // H-Bridge #11 corresponds to index 10
  delay(1); // 0.001s ON


  Serial.println("H-Bridge #11 OFF");
  setBridge(bridges[10], "off");
  delay(50);
  // set # 12 forward
  Serial.println("H-Bridge #12 FORWARD");
  setBridge(bridges[11], "forward"); // H-Bridge #12 corresponds to index 11
  delay(1); // 0.001s ON


  Serial.println("H-Bridge #12 OFF");
  setBridge(bridges[11], "off");
  delay(50);
  delay(500); // 4s OFF

  Serial.println("H-Bridge #9 REVERSE");
  setBridge(bridges[9], "reverse");
  delay(1); // 0.001s ON

  Serial.println("H-Bridge #9 OFF");
  setBridge(bridges[9], "off");
  delay(50);
  // set # 10 reverse
  Serial.println("H-Bridge #10 REVERSE");
  setBridge(bridges[10], "reverse"); // H-Bridge #10 corresponds to index 9

  delay(1); // 0.001s ON

  Serial.println("H-Bridge #10 OFF");
  setBridge(bridges[10], "off");
  delay(50);
  // set # 11 reverse

  Serial.println("H-Bridge #11 REVERSE");
  setBridge(bridges[11], "reverse"); // H-Bridge #11 corresponds to index 10

  delay(1); // 0.001s ON
  

  Serial.println("H-Bridge #11 OFF");
  setBridge(bridges[11], "off");
  

  delay(500); // 10s OFF
}

// === Control Functions ===

void initExpander(uint8_t addr) {
  writeRegister(addr, IODIRA, 0x00); // All A = output
  writeRegister(addr, IODIRB, 0x00); // All B = output
  writeRegister(addr, OLATA, 0x00);  // Clear outputs
  writeRegister(addr, OLATB, 0x00);
}

void setAllBridges(const char* mode) {
  for (int i = 0; i < 16; i++) {
    setBridge(bridges[10], mode);
  }
}

void setBridge(HBridgeControl hb, const char* mode) {
  if (strcmp(mode, "forward") == 0) {
    setPin(hb.expander, hb.phase_reg, hb.phase_bit, true);   // PH = 1
    setPin(hb.expander, hb.enable_reg, hb.enable_bit, false); // EN = 0
  } else if (strcmp(mode, "reverse") == 0) {
    setPin(hb.expander, hb.phase_reg, hb.phase_bit, false);  // PH = 0
    setPin(hb.expander, hb.enable_reg, hb.enable_bit, true);  // EN = 1
  } else { // "off"
    setPin(hb.expander, hb.phase_reg, hb.phase_bit, false);  // PH = 0
    setPin(hb.expander, hb.enable_reg, hb.enable_bit, false); // EN = 0 (Hi-Z)
  }
}

void setPin(uint8_t addr, uint8_t reg, uint8_t bit, bool value) {
  Serial.print("Setting pin: ");
  Serial.print("Addr: 0x");
  Serial.print(addr, HEX);
  Serial.print(", Reg: 0x");
  Serial.print(reg, HEX);
  Serial.print(", Bit: ");
  Serial.print(bit);
  Serial.print(", Value: ");
  Serial.println(value);

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
