/** Vibration Sensor Siren
 * 2. 진동 감지 알림 사이렌 만들기
 *  진동 감지시에 0.3초 초록색 LED가 켜지고 소리가 나며,
 *  0.2초 LED가 꺼지고 소리가 안 나게
 *  총 5초 동안 작동하는 프로그램을 만들어 주세요.
 */
int buzzer = 3;
int greenLed = 10;
int vibe = 11;

void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(vibe, INPUT);
}

void loop() {
  boolean isVibrate = digitalRead(vibe);
  if (isVibrate == true) {
    for (int cnt=0; cnt < 10; cnt++) {
      digitalWrite(greenLed, HIGH);
      tone(buzzer, 912, 300);
      delay(300);
      digitalWrite(greenLed, LOW);
      noTone(buzzer);
      delay(200);
    }
  }
}
