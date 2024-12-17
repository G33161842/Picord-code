#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid[] = {"daniu0807", "CHANG"};
const char* password[] = {"33161842", "063316756"};
const int wifiNetworks = 2;

const char* serverUrl = "https://3.1.140.221:10011/upload.php";
const int irPin1 = D5;
const int irPin2 = D6;
const int irPin3 = D7;
IRsend irsend1(irPin1);
IRsend irsend2(irPin2);
IRsend irsend3(irPin3);

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

  bool connected = false;
  for (int i = 0; i < wifiNetworks; i++) {
    Serial.print("Try to connect Wi-Fi: ");
    Serial.println(ssid[i]);
    WiFi.begin(ssid[i], password[i]);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      delay(1000);
      Serial.print(".");
      retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to ");
      Serial.println(ssid[i]);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Wi-Fi Connected!");
      display.print("IP Address: ");
      display.println(WiFi.localIP());
      display.display();
      connected = true;
      break;
    } else {
      Serial.println("\nConnection failed, try next");
    }
  }

  if (!connected) {
    Serial.println("Can't connect to any Wi-Fi!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wi-Fi failed!");
    display.display();
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Fan Control</title><style>body { font-family: Arial; text-align: center; }</style></head><body>";
    html += "<h1>Fan Control</h1>";
    html += "<button onclick=\"sendRequest('/on1')\">Fan 1 ON</button>";
    html += "<button onclick=\"sendRequest('/off1')\">Fan 1 OFF</button>";
    html += "<button onclick=\"sendRequest('/on2')\">Fan 2 ON</button>";
    html += "<button onclick=\"sendRequest('/off2')\">Fan 2 OFF</button>";
    html += "<button onclick=\"sendRequest('/on3')\">Fan 3 ON</button>";
    html += "<button onclick=\"sendRequest('/off3')\">Fan 3 OFF</button>";
    html += "<button onclick=\"sendRequest('/auto')\">Enable Auto</button>";
    html += "<button onclick=\"sendRequest('/manual')\">Manual Mode</button>";
    html += "<p id='status'>Fan Status: Unknown</p>";
    html += "<script>function sendRequest(url) { var xhr = new XMLHttpRequest(); xhr.open('GET', url, true); xhr.onload = function() { if (xhr.status === 200) { document.getElementById('status').innerText = xhr.responseText; } }; xhr.send(); }</script></body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/on1", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend1.sendSymphony(0xD82, 12);
    fan1 = 1;
    request->send(200, "text/plain", "Fan 1: ON");
  });
  server.on("/off1", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend1.sendSymphony(0xD81, 12);
    fan1 = 0;
    request->send(200, "text/plain", "Fan 1: OFF");
  });
  server.on("/on2", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend2.sendSymphony(0xD82, 12);
    fan2 = 1;
    request->send(200, "text/plain", "Fan 2: ON");
  });
  server.on("/off2", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend2.sendSymphony(0xD81, 12);
    fan2 = 0;
    request->send(200, "text/plain", "Fan 2: OFF");
  });
  server.on("/on3", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend3.sendSymphony(0xD82, 12);
    fan3 = 1;
    request->send(200, "text/plain", "Fan 3: ON");
  });
  server.on("/off3", HTTP_GET, [](AsyncWebServerRequest *request) {
    irsend3.sendSymphony(0xD81, 12);
    fan3 = 0;
    request->send(200, "text/plain", "Fan 3: OFF");
  });
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 1;
    request->send(200, "text/plain", "Auto Mode Enabled");
  });
  server.on("/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoMode = 0;
    request->send(200, "text/plain", "Manual Mode Enabled");
  });

  server.begin();
}

void loop() {
  if (autoMode == 1) {
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      Serial.print("Temperature: ");
      Serial.println(temperature);
      if (temperature >= thresholdTemphigh) {
        if (fan1 == 0) {
          irsend1.sendSymphony(0xD82, 12);
          fan1 = 1;
          delay(1000);
        }
        if (fan2 == 0) {
          irsend2.sendSymphony(0xD82, 12);
          fan2 = 1;
          delay(1000);
        }
        if (fan3 == 0) {
          irsend3.sendSymphony(0xD82, 12);
          fan3 = 1;
          delay(1000);
        }
      } else if (temperature < thresholdTemplow) {
        if (fan1 == 1) {
          irsend1.sendSymphony(0xD81, 12);
          fan1 = 0;
          delay(1000);
        }
        if (fan2 == 1) {
          irsend2.sendSymphony(0xD81, 12);
          fan2 = 0;
          delay(1000);
        }
        if (fan3 == 1) {
          irsend3.sendSymphony(0xD81, 12);
          fan3 = 0;
          delay(1000);
        }
      }
    } else {
      Serial.println("Failed to read temperature");
    }
    delay(10000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (!isnan(temp) && !isnan(humidity)) {
      String postData = "temperature=" + String(temp) + "&humidity=" + String(humidity);
      int httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        Serial.println(httpResponseCode);
        Serial.println(http.getString());
      } else {
        Serial.println("Failed to send DHT data");
      }
    } else {
      Serial.println("Failed to read DHT data");
    }
    http.end();
  }
}
