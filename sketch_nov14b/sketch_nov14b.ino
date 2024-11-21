#include <IRremoteESP8266.h>
#include <IRsend.h> // 修正为正确的头文件
#include <IRutils.h>

const int irPin = D5; // 红外发射器连接到 D5
IRsend irsend(irPin); // 初始化 IRsend，并传入对应的引脚

void setup() {
  Serial.begin(115200);
  irsend.begin(); // 初始化 IRsend
  Serial.println("IR sender initialized.");
}

void loop() {
  // 发送红外遥控码（例如发送一个特定的遥控器代码）
  Serial.println("Sending NEC code...");
  irsend.sendNEC(0xFF02FD, 32); // NEC 协议代码，32 表示 32 位的码
  delay(5000); // 每 5 秒发送一次信号
}
