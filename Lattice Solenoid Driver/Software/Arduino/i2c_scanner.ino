#include <Wire.h>

void setup() {
  Wire.begin();              // Join I2C bus
  Serial.begin(9600);        // Start serial communication
  while (!Serial);           // Wait for Serial Monitor to open (esp. useful on Mega)
  Serial.println("Scanning I2C devices...");
}

void loop() {
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
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found");
  else Serial.println("Scan complete.");

  delay(5000); // Scan again in 5 seconds
}
