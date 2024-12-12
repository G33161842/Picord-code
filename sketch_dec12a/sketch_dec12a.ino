#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>  // 引入 Wi-Fi 库

// 屏幕分辨率
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Wi-Fi 网络配置
const char* ssid = "daniu0807";  // 替换为你的Wi-Fi名称
const char* password = "33161842";  // 替换为你的Wi-Fi密码

// 创建 OLED 对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // 初始化串口监视器
  Serial.begin(115200);

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
}

void loop() {
  // 可以在此添加其他代码，比如定期显示数据等
}
