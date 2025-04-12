#include <Wire.h>

#define MCP23017_ADDR 0x20

// MCP23017 Register addresses
#define IODIRB 0x01  // I/O direction register for GPIOB
#define OLATB  0x15  // Output latch register for GPIOB

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor

  Serial.println("Running I2C scan...");
  scanI2CDevices();

  Serial.println("Initializing MCP23017...");

  // Set GPB0 and GPB1 as outputs (0 = output, 1 = input)
  writeRegister(MCP23017_ADDR, IODIRB, 0b11111100);
  Serial.println("Configured GPB0 and GPB1 as outputs.");
  delay(100);
}

void loop() {
  // Repeat I2C scan every 4 cycles
  static int cycleCount = 0;
  if (cycleCount++ % 4 == 0) {
    Serial.println("Checking I2C bus...");
    scanI2CDevices();
  }

  Serial.println("Setting (0, 0)");
  writeRegister(MCP23017_ADDR, OLATB, 0b00000000);
  delay(500);

  Serial.println("Setting (1, 0)");
  writeRegister(MCP23017_ADDR, OLATB, 0b00000001);
  delay(500);

  Serial.println("Setting (0, 1)");
  writeRegister(MCP23017_ADDR, OLATB, 0b00000010);
  delay(500);

  Serial.println("Setting (1, 1)");
  writeRegister(MCP23017_ADDR, OLATB, 0b00000011);
  delay(500);
}

// Writes a value to a register on the MCP23017
void writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// I2C scanner function
void scanI2CDevices() {
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found.");
  else Serial.println("I2C scan complete.");
}
