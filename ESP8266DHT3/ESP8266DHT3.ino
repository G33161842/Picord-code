#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>  // 引入 HTTPClient 库

#define DHTPIN 7          // DHT11 数据引脚
#define DHTTYPE DHT11     // 使用 DHT11 温湿度传感器

//const char* ssid = "picord";
//const char* password = "000010000";

const char* ssid = "daniu0807";
const char* password = "33161842";

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

bool autoControl = false; // 自动控制开关

void setup() {
  Serial.begin(115200);

  // 连接到Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("连接到Wi-Fi中");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  server.on("/auto", HTTP_GET, []() {
    autoControl = true;
    server.send(200, "text/plain", "Auto mode activated");
  });

  server.on("/disauto", HTTP_GET, []() {
    autoControl = false;
    server.send(200, "text/plain", "Auto mode deactivated");
  });

  server.begin();
}

void loop() {
  server.handleClient();

  // 只有在自动模式启用时，才根据温度控制 API
  if (autoControl) {
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
      Serial.println("读取温度失败！");
      return;
    }

    Serial.print("温度: ");
    Serial.println(temperature);

    if (temperature > 28) {
      sendApiRequest("https://redweb.magicboy.xyz/api/red3_on");
    } else if (temperature < 25) {
      sendApiRequest("https://redweb.magicboy.xyz/api/red3_off");
    }
  }
}

void sendApiRequest(const char* url) {
  WiFiClient client;
  HTTPClient http;  // 创建 HTTPClient 对象
  http.begin(client, url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.println("API 请求发送成功");
  } else {
    Serial.println("API 请求发送失败");
  }
  
  http.end();
}
