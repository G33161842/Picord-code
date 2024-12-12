#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>  // 引入 Wi-Fi 库
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <DHT.h>  // 引入 DHT 库

// 屏幕分辨率
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Wi-Fi 网络配置
//const char* ssid = "picord";  // 替换为你的Wi-Fi名称
//const char* password = "000010000";  // 替换为你的Wi-Fi密码

const char* ssid = "daniu0807";  // 替换为你的Wi-Fi名称
const char* password = "33161842";  // 替换为你的Wi-Fi密码

// 创建 OLED 对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// IR 发射器引脚设置
const int irPin = D5;
IRsend irsend(irPin);

// DHT 温度传感器设置
const int DHTPin = D4;  // DHT 感测器接脚
#define DHTTYPE DHT11  // 设置 DHT 型号 (DHT11 或 DHT22)
DHT dht(DHTPin, DHTTYPE);

// 变量状态
int fan = 0;  // 手动模式下风扇状态（1: 开, 0: 关）
int autoMode = 0;  // 自动模式状态（1: 开启, 0: 关闭）
const float thresholdTemphigh = 28.0;  // 临界温度 (28°C 可自行调整)
const float thresholdTemplow = 25.0;  // 临界温度 (25°C 可自行调整)

// 建立 Web 服务器
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  irsend.begin();
  dht.begin();  // 启动 DHT 传感器

  // 初始化 OLED 屏幕
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // 停止程序
  }
  display.clearDisplay();
  display.display();

  // 连接到 Wi-Fi
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to Wi-Fi...");
  display.display();

  WiFi.begin(ssid, password);  // 连接到 Wi-Fi

  // 等待连接
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // 连接成功
  Serial.println("Connected to WiFi");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wi-Fi Connected!");
  display.print("IP Address: ");
  display.println(WiFi.localIP());  // 显示 IP 地址
  display.display();

  // 设置网页路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>电风扇控制</title>";
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
  
    html += "<h1>电风扇控制</h1>";
    html += "<button class='button' onclick=\"sendRequest('/on1')\">手动开启风扇</button>";
    html += "<button class='button' onclick=\"sendRequest('/off1')\">手动关闭风扇</button>";
    html += "<button class='button' onclick=\"sendRequest('/auto')\">启用自动模式</button>";
    html += "<button class='button' onclick=\"sendRequest('/manual')\">切换手动模式</button>";
    html += "<p id='status'>电风扇状态：未知</p>";
  
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

  // 手动开启风扇
  server.on("/on1", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD82, 12);
    fan = 1;
    request->send(200, "text/plain", "电风扇状态：手动开启");
  });

  // 手动关闭风扇
  server.on("/off1", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend.sendSymphony(0xD81, 12);
    fan = 0;
    request->send(200, "text/plain", "电风扇状态：手动关闭");
  });

  // 启用自动模式
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 1;
    request->send(200, "text/plain", "自动模式已启用");
  });

  // 切换手动模式
  server.on("/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 0;
    request->send(200, "text/plain", "手动模式已启用");
  });

  // 启动服务器
  server.begin();
}

void loop() {
  if (autoMode == 1) {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      Serial.print("温度: ");
      Serial.println(temperature);

      // 根据温度判断是否开启/关闭风扇
      if (temperature >= thresholdTemphigh && fan == 0) {
        irsend.sendSymphony(0xD82, 12);  // 发送开启指令
        fan = 1;
        Serial.println("自动开启风扇");
      } else if (temperature < thresholdTemplow && fan == 1) {
        irsend.sendSymphony(0xD81, 12);  // 发送关闭指令
        fan = 0;
        Serial.println("自动关闭风扇");
      }
    } else {
      Serial.println("读取温度失败");
    }
    delay(10000);  // 每10秒检查一次温度
  }
}
