#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char* ssid = "applepie";      // WiFi名称
const char* password = "000010000";  // WiFi密码

WiFiUDP udp;  // 创建UDP实例

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);  // 连接到Wi-Fi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // 开始扫描局域网
  scanNetwork();
}

void loop() {
  // 可以在这里重复扫描网络
}

void scanNetwork() {
  IPAddress broadcastIp = WiFi.localIP();  // 获取当前设备的IP
  broadcastIp[3] = 255;  // 设置为广播地址

  udp.beginPacket(broadcastIp, 12345);  // 发送广播消息到端口 12345
  udp.write("Hello");  // 发送简单的消息
  udp.endPacket();  // 结束数据包

  // 监听设备的响应
  unsigned long start = millis();
  while (millis() - start < 5000) {  // 等待 5 秒钟响应
    int packetSize = udp.parsePacket();
    if (packetSize) {
      IPAddress remoteIp = udp.remoteIP();  // 获取响应设备的IP地址
      Serial.print("Device found: ");
      Serial.println(remoteIp);

      // 如果需要可以进行MAC地址的进一步比对
    }
  }
}
