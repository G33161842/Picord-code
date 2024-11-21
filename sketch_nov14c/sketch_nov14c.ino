#include <IRsend.h>
#include <IRremoteESP8266.h>

const uint16_t IR_PIN = D5;  // 發射模組連接的 GPIO
IRsend irsend(IR_PIN);

// 替換為你讀取到的信號
const uint64_t ON_SIGNAL = 0x20DF10EF;  // 開啟暖氣
const uint64_t OFF_SIGNAL = 0x20DF906F; // 關閉暖氣

const int BUTTON_PIN = D4; // 按鈕連接的 GPIO
bool heaterState = false;

void setup() {
    irsend.begin();
    pinMode(BUTTON_PIN, INPUT_PULLUP); // 啟用內建上拉電阻
    Serial.begin(115200);
    Serial.println("紅外線發射器準備中...");
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {  // 按鈕被按下
        delay(200);  // 去抖動
        heaterState = !heaterState;  // 切換狀態

        if (heaterState) {
            irsend.sendNEC(ON_SIGNAL, 32);  // 發送開啟信號
            Serial.println("暖氣已開啟");
        } else {
            irsend.sendNEC(OFF_SIGNAL, 32);  // 發送關閉信號
            Serial.println("暖氣已關閉");
        }

        delay(500);  // 防止多次觸發
    }
}
