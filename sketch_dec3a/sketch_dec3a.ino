#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Wi-Fi 設定
const char* ssid = "daniu0807";     // 替換為你的 WiFi SSID
const char* password = "33161842"; // 替換為你的 WiFi 密碼

// Web 伺服器
AsyncWebServer server(80);

// IR 發射器設定
const uint16_t kIrLedPin = D5; // 發射器連接的 GPIO 腳位
IRsend irsend(kIrLedPin);

void setup() {
  // 初始化串口監視器
  Serial.begin(115200);

  // 初始化 IR 發射器
  irsend.begin();

  // 連接到 Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("正在連接 Wi-Fi...");
  }
  Serial.println("Wi-Fi 連接成功！");
  Serial.println(WiFi.localIP());

  // 設定 Web 伺服器路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html lang="zh-TW">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP8266 電風扇控制</title>
        <style>
          body {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #f0f0f0;
            font-family: Arial, sans-serif;
          }
          h1 {
            margin-bottom: 30px;
          }
          button {
            width: 80%;
            max-width: 300px;
            height: 100px;
            font-size: 24px;
            margin: 20px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 10px;
            cursor: pointer;
          }
          button:active {
            background-color: #45a049;
          }
        </style>
      </head>
      <body>
        <h1>控制電風扇</h1>
        <button onclick="fetch('/on')">開啟電風扇</button>
        <button onclick="fetch('/off')">關閉電風扇</button>
      </body>
      </html>
    )rawliteral");
  });

  // 處理開啟電風扇的請求
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD82, 12);  // 發送 SYMPHONY 格式的 12 位訊號
    Serial.println("已發送開啟電風扇指令");
    request->send(200, "text/plain", "電風扇已開啟");
  });

  // 處理關閉電風扇的請求
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD81, 12); // 發送 SYMPHONY 格式的 12 位訊號
    Serial.println("已發送關閉電風扇指令");
    request->send(200, "text/plain", "電風扇已關閉");
  });

  // 啟動伺服器
  server.begin();
}

void loop() {
  // 不需要任何程式碼
}
