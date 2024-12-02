#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Wi-Fi 设置
const char* ssid = "daniu0807";
const char* password = "33161842";

// IR 发射器引脚设置
const int irPin = D5;
IRsend irsend(irPin);

// 创建 Web 服务器
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  irsend.begin();

  // 连接 Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("連接 WiFi中...");
  }
  Serial.println("已連接至 WiFi");

  // 获取并打印 ESP8266 的 IP 地址
  IPAddress ip = WiFi.localIP();
  Serial.print("ESP8266 IP Address: ");
  Serial.println(ip);

  // 设置网页路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html>";
    html += "<html><head><meta charset='UTF-8'><title>暖氣機控制</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; height: 100vh; display: flex; justify-content: center; align-items: center; }";
    html += "#container { text-align: center; }";
    html += "h1 { font-size: 3em; margin-bottom: 40px; }";
    html += "button { width: 300px; height: 100px; font-size: 2em; margin: 20px 0; background-color: #FF5733; color: white; border: none; border-radius: 10px; cursor: pointer; }";
    html += "button:hover { background-color: #E74C3C; }";
    html += "p { font-size: 1.5em; }";
    html += "</style></head>";
    html += "<body>";
    html += "<div id='container'>";
    html += "<h1>暖氣機控制</h1>";
    html += "<button onclick=\"sendRequest('/on')\">暖氣機 - 開</button><br>";
    html += "<button onclick=\"sendRequest('/off')\">暖氣機 - 關</button><br>";
    html += "<p id='status'>暖氣機狀態：未知</p>";
    html += "</div>";
    html += "<script>";
    html += "function sendRequest(url) {";
    html += "  var xhr = new XMLHttpRequest();";
    html += "  xhr.open('GET', url, true);";
    html += "  xhr.onload = function() {";
    html += "    if (xhr.status === 200) {";
    html += "      document.getElementById('status').innerHTML = xhr.responseText;";
    html += "    }";
    html += "  };";
    html += "  xhr.send();";
    html += "}";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // 開啟暖氣機
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendNEC(0xFF02FD, 32);  // 發送暖氣機開啟指令
    request->send(200, "text/plain", "暖氣機狀態：已開啟");
  });

  // 關閉暖氣機
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendNEC(0xFF02FD, 32);  // 發送暖氣機關閉指令（若開關使用相同訊號）
    request->send(200, "text/plain", "暖氣機狀態：已關閉");
  });

  // 启动服务器
  server.begin();
}

void loop() {
  // 空循环
}
