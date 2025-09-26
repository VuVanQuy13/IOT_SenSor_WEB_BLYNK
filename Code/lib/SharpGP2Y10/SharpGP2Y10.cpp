#include "SharpGP2Y10.h"

SharpGP2Y10::SharpGP2Y10(int mPin, int lPin) {
  measurePin = mPin;
  ledPower   = lPin;
  samplingTime = 280;
  deltaTime   = 40;
  sleepTime   = 9680;
}

void SharpGP2Y10::begin() {
  pinMode(ledPower, OUTPUT);
  digitalWrite(ledPower, HIGH); // Mặc định tắt LED
}


int SharpGP2Y10::readRaw() {
  digitalWrite(ledPower, LOW);                 // Bật LED (active LOW)
  delayMicroseconds(samplingTime);

  int voMeasured = analogRead(measurePin);    // Đọc ADC
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);                // Tắt LED
  delayMicroseconds(sleepTime);
  return voMeasured;
}

// Lấy giá trị Vôn
float SharpGP2Y10::readVoltage() {
  int raw = readRaw();
  return raw * (5.0 / 1024.0);   // Nếu dùng 3.3V thì sửa thành (3.3 / 4096.0) cho ESP32
}

// Lấy giá trị bụi
float SharpGP2Y10::readDustDensity() {
  float voltage = readVoltage();
  return 0.17 * voltage - 0.1;   // Công thức tuyến tính
}
