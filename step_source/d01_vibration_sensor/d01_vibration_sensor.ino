/** Vibration Sensor
 * 1. 진동 감지 센서
 */

int vibe = 11;

void setup() {
  Serial.begin(115200);
  pinMode(vibe, INPUT);
}

void loop() {
  Serial.print("Vibe Value: ");
  Serial.println(digitalRead(vibe));
}
