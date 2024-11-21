#include <IRremoteESP8266.h>
#include <IRsend.h>

const int irPin = D5; // 红外发射器连接到 D5
IRsend irsend(irPin); // 初始化 IRsend，并传入对应的引脚

void setup() {
  // Initialize the IR sender.
  irsend.begin();
  Serial.begin(115200);
  Serial.println("IR transmitter ready.");
}

void loop() {
  // Send the SYMPHONY protocol code: 0xD82 (12 bits).
  const uint16_t symphonyCode = 0xD82;
  const uint16_t numBits = 12;

  Serial.println("Sending SYMPHONY 0xD82...");
  irsend.sendSymphony(symphonyCode, numBits);
  delay(2000); // Wait 2 seconds before sending again.
}