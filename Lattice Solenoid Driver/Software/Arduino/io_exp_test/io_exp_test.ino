#include <Wire.h>

#define MCP1_ADDR 0x24
#define MCP2_ADDR 0x20

// MCP23017 register addresses (BANK=0 default)
#define IODIRA 0x01
#define IODIRB 0x00
#define OLATA  0x15
#define OLATB  0x14

bool outputHigh = true;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("Initializing MCP23017 expanders...");

  initExpander(MCP1_ADDR);
  initExpander(MCP2_ADDR);
}

void loop() {
  Serial.println("\nRunning I2C scan...");
  scanI2C();

  Serial.print("Setting all outputs to ");
  Serial.println(outputHigh ? "HIGH" : "LOW");

  writeAll(MCP1_ADDR, outputHigh ? 0xFF : 0x00);
  //writeAll(MCP2_ADDR, outputHigh ? 0xFF : 0x00);

  outputHigh = !outputHigh;
  delay(3000);
}

void initExpander(uint8_t addr) {
  // Set all A and B pins to output
  writeRegister(addr, IODIRA, 0x00);
  writeRegister(addr, IODIRB, 0x00);

  // Set all outputs LOW initially
  writeRegister(addr, OLATA, 0x00);
  writeRegister(addr, OLATB, 0x00);
}

void writeAll(uint8_t addr, uint8_t value) {
  writeRegister(addr, OLATA, value);
  writeRegister(addr, OLATB, value);
}

void writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void scanI2C() {
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  ✓");
      nDevices++;
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found.");
}
