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
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // 获取并打印 ESP8266 的 IP 地址
  IPAddress ip = WiFi.localIP();
  Serial.print("ESP8266 IP Address: ");
  Serial.println(ip);

  // 设置网页路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html>";
    html += "<html><head><meta charset='UTF-8'><title>電風扇控制</title></head><body>";
    html += "<h1>電風扇控制</h1>";
    html += "<button onclick=\"sendRequest('/on')\">電風扇-開(強中弱)</button><br><br>";
    html += "<button onclick=\"sendRequest('/off')\">電風扇-關</button><br><br>";
    html += "<p id='status'>電風扇狀態：未知</p>";
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

  // 开启电风扇
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD82, 12);  // 替换为你捕获的遥控器编码
    request->send(200, "text/plain", "電風扇狀態：已開啟");
  });

  // 关闭电风扇
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD81, 12);  // 替换为你捕获的遥控器编码
    request->send(200, "text/plain", "電風扇狀態：已關閉");
  });

  // 启动服务器
  server.begin();
}

void loop() {
  // 空循环
}
