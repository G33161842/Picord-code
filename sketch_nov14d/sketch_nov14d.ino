#include <IRremoteESP8266.h>
#include <IRsend.h>

const int irPin = D5;    // 紅外發射器連接到 D5
const int buttonPin = D2; // 按鈕連接到 D6

IRsend irsend(irPin);    // 初始化 IRsend，並傳入對應的引腳

unsigned long lastDebounceTime = 0; // 去彈跳計時
const unsigned long debounceDelay = 50; // 去彈跳延遲 (毫秒)
bool buttonState = LOW;   // 按鈕當前狀態
bool lastButtonState = LOW; // 上一次的按鈕狀態

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // 設置按鈕引腳為輸入，內建上拉電阻
  irsend.begin();                  // 初始化紅外發射器
  Serial.begin(115200);
  Serial.println("IR transmitter with button control ready.");
}

void loop() {
  int reading = digitalRead(buttonPin); // 讀取按鈕狀態

  // 檢查是否需要進行去彈跳處理
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // 更新去彈跳計時
  }

  // 如果經過了去彈跳時間，確認按鈕狀態變化
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // 只有當按鈕狀態改變時，才更新狀態
    if (reading != buttonState) {
      buttonState = reading;

      // 如果按鈕被按下（LOW -> HIGH），發送紅外信號
      if (buttonState == LOW) {
        const uint16_t symphonyCode = 0xD82; // SYMPHONY協議的12位代碼
        const uint16_t numBits = 12;

        Serial.println("Button pressed. Sending SYMPHONY 0xD82...");
        irsend.sendSymphony(symphonyCode, numBits);
      }
    }
  }

  lastButtonState = reading; // 更新上一個按鈕狀態
}
