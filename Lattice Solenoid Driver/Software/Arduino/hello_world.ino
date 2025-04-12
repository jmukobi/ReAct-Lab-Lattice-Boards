void setup() {
    Serial.begin(9600); // Start the serial communication
    while (!Serial);    // Wait for Serial Monitor to open (on boards like Leonardo/Mega)
    Serial.println("Hello, world!");
  }
  
  void loop() {
    delay(1000); // Wait 1 second
    Serial.println("Hello, world!");
  }
  