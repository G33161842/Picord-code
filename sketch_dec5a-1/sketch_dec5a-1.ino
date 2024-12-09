#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <DHT.h>  // 引入 DHT 庫

// Wi-Fi 設定
const char* ssid = "daniu0807";
const char* password = "33161842";

// IR 發射器引腳設定
const int irPin = D5;
IRsend irsend(irPin);

// DHT 溫度感測器設定
const int DHTPin = D2;      // DHT 感測器接腳
#define DHTTYPE DHT11       // 設定 DHT 型號 (DHT11 或 DHT22)
DHT dht(DHTPin, DHTTYPE);

// 變數狀態
int fan = 0;   // 手動模式下風扇狀態（1: 開, 0: 關）
int autoMode = 0; // 自動模式狀態（1: 開啟, 0: 關閉）
const float thresholdTemphigh = 28.0;  // 臨界溫度 (28°C 可自行調整)
const float thresholdTemplow = 25.0;  // 臨界溫度 (25°C 可自行調整)

// 建立 Web 伺服器
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  irsend.begin();
  dht.begin();  // 啟動 DHT 感測器

  // 連接 Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("連接 WiFi中...");
  }
  Serial.println("已連接至 WiFi");

  // 獲取並打印 ESP8266 的 IP 地址
  IPAddress ip = WiFi.localIP();
  Serial.print("ESP8266 IP Address: ");
  Serial.println(ip);

  // 設置網頁路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>電風扇控制</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; background-color: #f5f5f5; }";
    html += "h1 { color: #333; }";
    html += ".button {";
    html += "  display: block; width: 100%; max-width: 300px; padding: 20px; margin: 10px auto;";
    html += "  font-size: 24px; color: white; background-color: #28a745; border: none; border-radius: 10px;";
    html += "  cursor: pointer; text-align: center; }";
    html += ".button:hover { background-color: #218838; }";
    html += "#status { font-size: 20px; margin-top: 20px; color: #333; }";
    html += "#temperature { font-size: 20px; margin-top: 20px; color: #333; }";
    html += "#humidity { font-size: 20px; margin-top: 20px; color: #333; }";
    html += "</style></head><body>";

    html += "<h1>電風扇控制</h1>";
    html += "<button class='button' onclick=\"sendRequest('/on')\">手動開啟風扇</button>";
    html += "<button class='button' onclick=\"sendRequest('/off')\">手動關閉風扇</button>";
    html += "<button class='button' onclick=\"sendRequest('/auto')\">啟用自動模式</button>";
    html += "<button class='button' onclick=\"sendRequest('/manual')\">切換手動模式</button>";

    html += "<p id='status'>電風扇狀態：未知</p>";
    html += "<p id='temperature'>溫度：未知</p>";
    html += "<p id='humidity'>濕度：未知</p>";

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
    
    html += "function updateTemperature() {";
    html += "  var xhr = new XMLHttpRequest();";
    html += "  xhr.open('GET', '/temperature', true);";
    html += "  xhr.onload = function() {";
    html += "    if (xhr.status === 200) {";
    html += "      document.getElementById('temperature').innerHTML = '溫度：' + xhr.responseText + '°C';";
    html += "    }";
    html += "  };";
    html += "  xhr.send();";
    html += "}";
    
    html += "function updateHumidity() {";
    html += "  var xhr = new XMLHttpRequest();";
    html += "  xhr.open('GET', '/humidity', true);";
    html += "  xhr.onload = function() {";
    html += "    if (xhr.status === 200) {";
    html += "      document.getElementById('humidity').innerHTML = '濕度：' + xhr.responseText + '%';";
    html += "    }";
    html += "  };";
    html += "  xhr.send();";
    html += "}";
    
    html += "setInterval(updateTemperature, 10000);";
    html += "setInterval(updateHumidity, 10000);";
    html += "</script></body></html>";

    request->send(200, "text/html", html);
  });

  // 手動開啟風扇
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD82, 12);
    fan = 1;
    request->send(200, "text/plain", "電風扇狀態：手動開啟");
  });

  // 手動關閉風扇
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD81, 12);
    fan = 0;
    request->send(200, "text/plain", "電風扇狀態：手動關閉");
  });

  // 啟用自動模式
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 1;
    request->send(200, "text/plain", "自動模式已啟用");
  });

  // 切換手動模式
  server.on("/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 0;
    request->send(200, "text/plain", "手動模式已啟用");
  });

  // 顯示溫度
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      String temp = String(temperature);
      request->send(200, "text/plain", temp);
    } else {
      request->send(500, "text/plain", "溫度讀取失敗");
    }
  });

  // 顯示濕度
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    float humidity = dht.readHumidity();
    if (!isnan(humidity)) {
      String hum = String(humidity);
      request->send(200, "text/plain", hum);
    } else {
      request->send(500, "text/plain", "濕度讀取失敗");
    }
  });

  // 啟動伺服器
  server.begin();
}

void loop() {
  if (autoMode == 1) {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      Serial.print("溫度: ");
      Serial.println(temperature);

      if (temperature >= thresholdTemphigh && fan == 0) {
        irsend.sendSymphony(0xD82, 12);  // 發送開啟指令
        fan = 1;
        Serial.println("自動開啟風扇");
      } else if (temperature < thresholdTemplow && fan == 1) {
        irsend.sendSymphony(0xD81, 12);  // 發送關閉指令
        fan = 0;
        Serial.println("自動關閉風扇");
      }
    } else {
      Serial.println("讀取溫度失敗");
    }
    delay(10000);  // 每10秒檢查一次溫度
  }
}
