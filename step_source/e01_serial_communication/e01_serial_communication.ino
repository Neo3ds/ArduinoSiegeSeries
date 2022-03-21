/** Serial Communication
 * 1. 시리얼 통신을 통해 LED 제어
 *  시리얼 통신을 통해서 센서, 모듈들을 제어할 수도 있습니다.
 *  시리얼 모니터에 0이라는 문자를 작성하면 초록 LED가 꺼지고,
 *  1이라는 문자를 작성하면 초록 LED가 켜지게 해보겠습니다.
 */
int greenLed = 10;

void setup() {
  Serial.begin(115200);
  pinMode(greenLed, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '0') {
      digitalWrite(greenLed, LOW);
    } else if (cmd == '1') {
      digitalWrite(greenLed, HIGH);
    }
  }
}
