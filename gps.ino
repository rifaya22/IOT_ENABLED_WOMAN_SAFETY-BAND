#include <Wire.h>

const int MPU_addr = 0x68; // MPU6050 I2C address
const int buzzerPin = 9;

float lowerThreshold = 0.8;  // lower G threshold for fall detection
float upperThreshold = 2.0;  // upper G threshold for fall detection
unsigned long fallDetectionTime = 0;
bool fallDetected = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  // Wake the MPU6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);
}

void loop() {
  int16_t ax, ay, az;
  // Read accelerometer data
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
  // Convert raw data to g
  float x = ax / 16384.0;
  float y = ay / 16384.0;
  float z = az / 16384.0;

  // Calculate acceleration magnitude
  float magnitude = sqrt(x * x + y * y + z * z);

  Serial.print("Magnitude: "); Serial.println(magnitude, 2);

  if (magnitude < lowerThreshold || magnitude > upperThreshold) {
    if (!fallDetected) {
      fallDetected = true;
      fallDetectionTime = millis();
      // Activate buzzer and alert
      tone(buzzerPin, 2000);
      Serial.println("Fall detected! Buzzer ON");
    }
  } else {
    if (fallDetected && (millis() - fallDetectionTime > 2000)) {
      fallDetected = false;
      noTone(buzzerPin);
      Serial.println("Normal state. Buzzer OFF");
    }
  }
  delay(200);
}
