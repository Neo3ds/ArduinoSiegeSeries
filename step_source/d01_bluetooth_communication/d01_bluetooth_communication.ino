/** Bluetooth Communication
 * 1. 블루투스 통신을 통해 LED 제어
 *  블루투스 통신을 통해서 센서, 모듈들을 제어할 수도 있습니다.
 *  시리얼 모니터를 통하면 선이라는 제한점이 있지만,
 *  블루투스 통신을 통하면 선 없이도 제어할 수 있다는 이점이 있습니다.
 */

#include <SoftwareSerial.h>

SoftwareSerial BT(A4, A5); // TX, RX 
int greenLed = 10;

void setup() {
  Serial.begin(115200);
  BT.begin(115200);
  pinMode(greenLed, OUTPUT);
}

void loop() {
  if (BT.available()) {
    char cmd = BT.read();
    Serial.println(cmd);
    if (cmd == '0') {
      digitalWrite(greenLed, LOW);
      BT.write("OFF");
    } else if (cmd == '1') {
      digitalWrite(greenLed, HIGH);
      BT.write("ON");
    }
  }
}
