這是滑板大牛所需要的程式碼放置區

-------------------------------------------------------------------------

學習和發送紅外線訊號
學習訊號（如果奧運暖氣設備的紅外線代碼）
連接紅外線接收器模組後，執行以下程式碼：
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = D5; // 接收引脚

IRrecv irrecv(kRecvPin);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn(); // 启用接收
  Serial.println("红外接收器已启用");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(resultToHumanReadableBasic(&results)); // 打印红外信号
    irrecv.resume(); // 准备接收下一个信号
  }
}

按一下按鍵，記錄訊號資料（如協定和十六進位代碼）。

-------------------------------------------------------------------------

發送訊號
使用記錄的數據發送訊號：

#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = D4; // 红外发射引脚

IRsend irsend(kIrLed);

void setup() {
  irsend.begin();
  Serial.begin(115200);
  Serial.println("红外发射器已启用");
}

void loop() {
  // 示例信号：根据记录替换协议和数据
  irsend.sendNEC(0x20DF10EF, 32); // 使用NEC协议发送32位数据
  delay(5000); // 每5秒发送一次
}

-------------------------------------------------------------------------
