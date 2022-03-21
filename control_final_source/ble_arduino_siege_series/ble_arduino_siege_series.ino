/*
 * Author: TaeJune Joung
 * Enterprise: NEO3DS
 * Date: 22.01.17
 * Update: 22.03.21
 * License:
 *  본 코드의 저작권은 위와 같으며,
 *  상업적인 무단 도용시 법적인 책임을 물을 수 있음을 공지합니다.
 */

#include <SoftwareSerial.h>

/* 블루투스 핀 */
#define BT_TX            A4
#define BT_RX            A5

/* 구동 모터 */
#define LEFT_MOTOR_FW     7
#define LEFT_MOTOR_BW     8
#define RIGHT_MOTOR_FW    2
#define RIGHT_MOTOR_BW    4

#define LEFT_MOTOR_PWM    6
#define RIGHT_MOTOR_PWM   5

/* RGB 핀 */
#define BLUE_RGB         11

/* 버저 핀 */
#define BUZZER_PIN        3
/* 진동센서 핀 */
#define VIBE_PIN         A0

/* 장전 모터 */
#define GEAR_MOTOR_FW     9
#define GEAR_MOTOR_BW    10

/* 모터 속도 */
int motorSpeed = 255;
int GEAR_MAX_SPEED = 255;
int GEAR_MIN_SPEED = 0;
int GEAR_CATCH_SPEED = 50;

int life = 3;   // 생명수
char cmd;       // 블루투스 명령어

SoftwareSerial BT(BT_TX, BT_RX);

void setup() {
  /* 블루투스 통신 BAUD Rate */
  BT.begin(115200);

  /* PinMode */
  pinMode(LEFT_MOTOR_FW, OUTPUT);
  pinMode(LEFT_MOTOR_BW, OUTPUT);
  pinMode(RIGHT_MOTOR_FW, OUTPUT);
  pinMode(RIGHT_MOTOR_BW, OUTPUT);
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BLUE_RGB, OUTPUT);
  pinMode(GEAR_MOTOR_FW, OUTPUT);
  pinMode(GEAR_MOTOR_BW, OUTPUT);
  pinMode(VIBE_PIN, INPUT);

  digitalWrite(BLUE_RGB, HIGH); // FULL HP

  /* 작동 소리 */
  tone(BUZZER_PIN, 912, 100);
  delay(150);
  tone(BUZZER_PIN, 912, 100);
  delay(150);

  /* 구동 모터 PWM제어 */
  analogWrite(LEFT_MOTOR_PWM, motorSpeed);
  analogWrite(RIGHT_MOTOR_PWM, motorSpeed);
}

/**
 * motor_cntl
 * : 구동 모터 방향 제어 함수
 * @param(boolean lfw, boolean lbw, boolean rfw, boolean rbw)
 * 
 * cf) 모터를 끼우는 위치에 따라 전/후, 좌/우가 바뀔 수 있으니 이후 코드를 변경해주면 됩니다.
 */
void motor_cntl(boolean lfw, boolean lbw, boolean rfw, boolean rbw) {
  digitalWrite(LEFT_MOTOR_FW, lfw);
  digitalWrite(LEFT_MOTOR_BW, lbw);
  digitalWrite(RIGHT_MOTOR_FW, rfw);
  digitalWrite(RIGHT_MOTOR_BW, rbw);
}

/**
 * gear_motor_cntl
 * : 장전 모터 제어 함수
 * @param(int gfw, int gbw)
 */
void gear_motor_cntl(int gfw, int gbw) {
  analogWrite(GEAR_MOTOR_FW, gfw);
  analogWrite(GEAR_MOTOR_BW, gbw);
}

/**
 * detect_vibe
 * : 진동센서 감지 함수
 * @param(char v)
 */
void detect_vibe(char v) {
  unsigned int vibe_value = 0;
  while (cmd == v && life > 0) {
    vibe_value =  digitalRead(VIBE_PIN);
    if (vibe_value == 1) {
      life--;
      BT.write('h');

      for(int i=0; i < 2; i++) {
        tone(BUZZER_PIN, 194, 100);
        delay(150);
        tone(BUZZER_PIN, 212, 100);
        delay(150);
      }
      delay(1500);
      BT.write('g');
      break;
    }
    bluetooth_read();
  }
}

/**
 * bluetooth_read
 * : 블루투스 명령어 수신 및 
 */
void bluetooth_read() {
  if(BT.available()) {

    cmd = BT.read();
    
    if (cmd == '0') {           // 정지
      motor_cntl(0, 0, 0, 0);
    } else if (cmd == '1') {    // 전진
      motor_cntl(0, 1, 0, 1);
    } else if (cmd == '2') {    // 좌회전
      motor_cntl(0, 1, 1, 0);
    } else if (cmd == '3') {    // 우회전
      motor_cntl(1, 0, 0, 1);
    } else if (cmd == '4') {    // 후진
      motor_cntl(1, 0, 1, 0);
    } else if (cmd == '5') {    // 장전
      gear_motor_cntl(GEAR_MAX_SPEED, GEAR_MIN_SPEED);
    } else if (cmd == '6') {    // 장전Stop
      gear_motor_cntl(GEAR_CATCH_SPEED, GEAR_MIN_SPEED);
    } else if (cmd == '7') {    // 발사
      gear_motor_cntl(GEAR_MIN_SPEED, GEAR_MAX_SPEED);
      delay(200);
      gear_motor_cntl(GEAR_MIN_SPEED, GEAR_MIN_SPEED);
      BT.write('s');
    } else if (cmd == 's') {
      delay(3000);
      detect_vibe(cmd);
    } else if (cmd == 'r') {
      life = 3;
    }
  }
}

void loop() {
  bluetooth_read();
  
  if (life == 0) {
    digitalWrite(BLUE_RGB, LOW);
    BT.write('s');
    for(int i=0; i < 10; i++) {
      tone(BUZZER_PIN, 33, 300);
      delay(350);
    }
    life -= 1;
  } else if (life == 3) {
    analogWrite(BLUE_RGB, 255);
  } else if (life == 2) {
    analogWrite(BLUE_RGB, 168);
  } else if (life == 1) {
    analogWrite(BLUE_RGB, 84);
  }
}
