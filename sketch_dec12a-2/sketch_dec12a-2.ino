#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "picord";
const char* password = "000010000";

//const char* ssid = "daniu0807";
//const char* password = "33161842";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const int irPin1 = D5;
const int irPin2 = D6;
const int irPin3 = D7;
IRsend irsend1(irPin1);
IRsend irsend2(irPin2);
IRsend irsend3(irPin3);

const int DHTPin = D4;
#define DHTTYPE DHT11
DHT dht(DHTPin, DHTTYPE);

int fan1 = 0;
int fan2 = 0;
int fan3 = 0;
int autoMode = 0;
const float thresholdTemphigh = 28.0;
const float thresholdTemplow = 25.0;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  irsend1.begin();
  irsend2.begin();
  irsend3.begin();
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to Wi-Fi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wi-Fi Connected!");
  display.print("IP Address: ");
  display.println(WiFi.localIP());
  display.display();

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
    html += "</style></head><body>";

    html += "<h1>電風扇控制</h1>";
    html += "<button class='button' onclick=\"sendRequest('/on1')\">風扇1開啟</button>";
    html += "<button class='button' onclick=\"sendRequest('/off1')\">風扇1關閉</button>";
    html += "<button class='button' onclick=\"sendRequest('/on2')\">風扇2開啟</button>";
    html += "<button class='button' onclick=\"sendRequest('/off2')\">風扇2關閉</button>";
    html += "<button class='button' onclick=\"sendRequest('/on3')\">風扇3開啟</button>";
    html += "<button class='button' onclick=\"sendRequest('/off3')\">風扇3關閉</button>";
    html += "<button class='button' onclick=\"sendRequest('/auto')\">啟用自動模式</button>";
    html += "<button class='button' onclick=\"sendRequest('/manual')\">切換手動模式</button>";
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
    html += "</script></body></html>";

    request->send(200, "text/html", html);
  });

  server.on("/on1", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend1.sendSymphony(0xD82, 12);
    fan1 = 1;
    request->send(200, "text/plain", "風扇1狀態：開啟");
  });

  server.on("/off1", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend1.sendSymphony(0xD81, 12);
    fan1 = 0;
    request->send(200, "text/plain", "風扇1狀態：關閉");
  });

  server.on("/on2", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend2.sendSymphony(0xD82, 12);
    fan2 = 1;
    request->send(200, "text/plain", "風扇2狀態：開啟");
  });

  server.on("/off2", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend2.sendSymphony(0xD81, 12);
    fan2 = 0;
    request->send(200, "text/plain", "風扇2狀態：關閉");
  });

  server.on("/on3", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend3.sendSymphony(0xD82, 12);
    fan3 = 1;
    request->send(200, "text/plain", "風扇3狀態：開啟");
  });

  server.on("/off3", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend3.sendSymphony(0xD81, 12);
    fan3 = 0;
    request->send(200, "text/plain", "風扇3狀態：關閉");
  });

  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 1;
    request->send(200, "text/plain", "自動模式已啟用");
  });

  server.on("/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 0;
    request->send(200, "text/plain", "手動模式已啟用");
  });

  server.begin();
}

void loop() {
  if (autoMode == 1) {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      Serial.print("溫度: ");
      Serial.println(temperature);

      // 自動模式依序控制風扇
      if (temperature >= thresholdTemphigh) {
        if (fan1 == 0) {
          irsend1.sendSymphony(0xD82, 12);
          fan1 = 1;
          Serial.println("自動開啟風扇1");
          delay(1000);  // 避免信號重疊
        }
        if (fan2 == 0) {
          irsend2.sendSymphony(0xD82, 12);
          fan2 = 1;
          Serial.println("自動開啟風扇2");
          delay(1000);
        }
        if (fan3 == 0) {
          irsend3.sendSymphony(0xD82, 12);
          fan3 = 1;
          Serial.println("自動開啟風扇3");
          delay(1000);
        }
      } else if (temperature < thresholdTemplow) {
        if (fan1 == 1) {
          irsend1.sendSymphony(0xD81, 12);
          fan1 = 0;
          Serial.println("自動關閉風扇1");
          delay(1000);
        }
        if (fan2 == 1) {
          irsend2.sendSymphony(0xD81, 12);
          fan2 = 0;
          Serial.println("自動關閉風扇2");
          delay(1000);
        }
        if (fan3 == 1) {
          irsend3.sendSymphony(0xD81, 12);
          fan3 = 0;
          Serial.println("自動關閉風扇3");
          delay(1000);
        }
      }
    } else {
      Serial.println("讀取溫度失敗");
    }

    // 增加延遲，避免頻繁讀取或發送訊號
    delay(10000);
  }
}