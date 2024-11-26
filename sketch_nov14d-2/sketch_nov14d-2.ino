#include <IRremoteESP8266.h>
#include <IRsend.h>

// 引脚定义
const int irPin = D5;           // 红外发射器连接到 D5
const int openButtonPin = D7;   // "开" 按钮连接到 D
const int closeButtonPin = D6;  // "关" 按钮连接到 D

// 初始化 IRsend
IRsend irsend(irPin);

void setup() {
  // 初始化 IR 发射器
  irsend.begin();
  
  // 初始化按钮引脚
  pinMode(openButtonPin, INPUT_PULLUP);
  pinMode(closeButtonPin, INPUT_PULLUP);

  // 初始化串口通讯
  Serial.begin(115200);
  Serial.println("IR transmitter ready. Waiting for button presses...");
}

void loop() {
  // 检查 "开" 按钮是否被按下
  if (digitalRead(openButtonPin) == HIGH) { // 按钮按下时为 HIGH 要注意這邊
    Serial.println("Sending SYMPHONY 0xD82 (OPEN)..."); // 调试信息
    irsend.sendSymphony(0xD82, 12); // 发送开信号
    delay(500); // 防止多次触发
  }

  // 检查 "关" 按钮是否被按下
  if (digitalRead(closeButtonPin) == HIGH) { // 按钮按下时为 HIGH 要注意這邊
    Serial.println("Sending SYMPHONY 0xD81 (CLOSE)..."); // 调试信息
    irsend.sendSymphony(0xD81, 12); // 发送关信号
    delay(500); // 防止多次触发
  }
}
