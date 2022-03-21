/*
 * 아두이노 투석기 Blockly 코드
 * Author: TaeJune Joung
 * Enterprise: NEO3DS
 * Date: 22.01.17
 * License:
 *  본 코드의 저작권은 위와 같으며,
 *  상업적인 무단 도용시 법적인 책임을 물을 수 있음을 공지합니다.
 */

/* 라이브러리 import */
#include <SoftwareSerial.h>

/* 동작 상수 */
#define ALIVE          0
#define ANALOG         1

/* 상태 상수 */
#define GET            1
#define SET            2
#define RESET          3

/* 
 *전역변수 선언
 */
 
/* 블루투스 핀 */
#define PIN_BLE_TX            A4
#define PIN_BLE_RX            A5

/* 투석기 구동 모터 */
#define PIN_LEFT_MOTOR_FW      7
#define PIN_LEFT_MOTOR_BW      8
#define PIN_RIGHT_MOTOR_FW     2
#define PIN_RIGHT_MOTOR_BW     4

#define PIN_LEFT_MOTOR_PWM     6
#define PIN_RIGHT_MOTOR_PWM    5

/* RGB 핀 */
#define PIN_RGB_RED            9
#define PIN_RGB_GREEN         10
#define PIN_RGB_BLUE          11

/* 버저 핀 */
#define PIN_BUZZER             3
/* 진동센서 핀 */
#define PIN_VIBE              A0

/* 투석기 장전 모터 */
#define PIN_GEAR_MOTOR_FW     A1
#define PIN_GEAR_MOTOR_BW     A2

/* 투석기 구동 모터 속도 */
int motorSpeed = 255;

union {
  byte byteVal[4];
  float floatVal;
  long longVal;
} val;

union {
  byte byteVal[2];
  short shortVal;
} valShort;

const int START_VAL = 255;
const int DATA_LEN = 13;
const int END_VALUE = 10;

int beforeBuzNote = 0;
int rotation = 1000;

unsigned char buf[DATA_LEN];
unsigned char prevc=0;
int index = 0;
boolean isStart = false;

/* 라이브러리 사용 */
SoftwareSerial BT(PIN_BLE_TX, PIN_BLE_RX);

void initPorts() {
  for (int i=2; i < 14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void updateDigitalPort(unsigned char c) {
  if (c == DATA_LEN && isStart == false) {
    if (prevc == START_VAL) {
      isStart = true;
      index = 1;
      writeBuffer(index-1, prevc);
      writeBuffer(index, c);
    }
  } else {
    prevc = c;
    if (isStart) {
      writeBuffer(index, c);
    }
  }

  index++;
  
  if (index > DATA_LEN) {
    isStart = false;
    index = 0;
  }

  if (isStart && index == DATA_LEN && index > 3) {
    isStart = false;
    parseData();
    index = 0;
  }
}

boolean isCheckOk () {
  if (buf[DATA_LEN-1] == END_VALUE) {
    int checkSum = buf[DATA_LEN-2];
    int sumValue = 0;
    for (int i=2; i < DATA_LEN-2; i++) {
      sumValue += buf[i];
    }
    sumValue = sumValue & 0xff;
    if (checkSum == sumValue) {
      return true;
    }
  }
  return false;
}

unsigned char readBuffer(int index) {
  return buf[index];
}

void buzCtrl(int buzOctave, int buzNote) {
  int note = buzOctave * 256 + buzNote;
  if (beforeBuzNote != note) {
    beforeBuzNote = note;
    if (note > 30) {
      tone(PIN_BUZZER, note);
    } else {
      noTone(PIN_BUZZER);
    }
  }
}

void setMotorSpeed(int speed) {
  analogWrite(PIN_RIGHT_MOTOR_PWM, speed);
  analogWrite(PIN_LEFT_MOTOR_PWM, speed);
}

void motorCtrl(int motorState) {
  switch(motorState) {
    case 0: //Stop
      digitalWrite(PIN_RIGHT_MOTOR_FW, LOW);
      digitalWrite(PIN_RIGHT_MOTOR_BW, LOW);
      digitalWrite(PIN_LEFT_MOTOR_FW, LOW);
      digitalWrite(PIN_LEFT_MOTOR_BW, LOW);
      analogWrite(PIN_RIGHT_MOTOR_PWM, 0);
      analogWrite(PIN_LEFT_MOTOR_PWM, 0);
      break;
    case 1: // Forward
      digitalWrite(PIN_RIGHT_MOTOR_FW, HIGH);
      digitalWrite(PIN_RIGHT_MOTOR_BW, LOW);
      digitalWrite(PIN_LEFT_MOTOR_FW, HIGH);
      digitalWrite(PIN_LEFT_MOTOR_BW, LOW);
      break;
    case 2: // Turn Left
      digitalWrite(PIN_RIGHT_MOTOR_FW, HIGH);
      digitalWrite(PIN_RIGHT_MOTOR_BW, LOW);
      digitalWrite(PIN_LEFT_MOTOR_FW, LOW);
      digitalWrite(PIN_LEFT_MOTOR_BW, HIGH);
      break;
    case 3: // Turn Right
      digitalWrite(PIN_RIGHT_MOTOR_FW, LOW);
      digitalWrite(PIN_RIGHT_MOTOR_BW, HIGH);
      digitalWrite(PIN_LEFT_MOTOR_FW, HIGH);
      digitalWrite(PIN_LEFT_MOTOR_BW, LOW);
      break;
    case 4: // Backward
      digitalWrite(PIN_RIGHT_MOTOR_FW, LOW);
      digitalWrite(PIN_RIGHT_MOTOR_BW, HIGH);
      digitalWrite(PIN_LEFT_MOTOR_FW, LOW);
      digitalWrite(PIN_LEFT_MOTOR_BW, HIGH);
      break;
  }
}

void rgbLedCtrl(int redValue, int greenValue, int blueValue) {
  if (redValue == 255) {
    digitalWrite(PIN_RGB_RED, HIGH);
  } else if (redValue == 0) {
    digitalWrite(PIN_RGB_RED, LOW);
  } else {
    analogWrite(PIN_RGB_RED, redValue);
  }

  if (greenValue == 255) {
    digitalWrite(PIN_RGB_GREEN, HIGH);
  } else if (greenValue == 0) {
    digitalWrite(PIN_RGB_GREEN, LOW);
  } else {
    analogWrite(PIN_RGB_GREEN, greenValue);
  }

  if (blueValue == 255) {
    digitalWrite(PIN_RGB_BLUE, HIGH);
  } else if (blueValue == 0) {
    digitalWrite(PIN_RGB_BLUE, LOW);
  } else {
    analogWrite(PIN_RGB_BLUE, blueValue);
  }
}

void parseData() {
  if (isCheckOk()) {
    int buzOctave = readBuffer(2);
    int buzNote = readBuffer(3);
    int motorState = readBuffer(4);
    int speed = readBuffer(5);
    int rgbRedValue = readBuffer(6);
    int rgbGreenValue = readBuffer(7);
    int rgbBlueValue = readBuffer(8);
    
    buzCtrl(buzOctave, buzNote);
    setMotorSpeed(speed);
    motorCtrl(motorState);
    rgbLedCtrl(rgbRedValue, rgbGreenValue, rgbBlueValue);
  }
}

int sendSumValue(int type, int value) {
  return (type + value) & 0xff;
}

void getVibeValue() {
  writeValue(0xff);
  writeValue(0x7);
  writeValue(ANALOG);
  int vibeValue = analogRead(PIN_VIBE);
  
  int largeVibeValue = vibeValue >> 8;
  int smallVibeValue = vibeValue & 0b11111111;
  
  writeValue(largeVibeValue);
  writeValue(smallVibeValue);
  
  int checkSum = 0;
  checkSum = (ANALOG + largeVibeValue + smallVibeValue) & 0xff;
  writeValue(checkSum);
  writeEnd();
}

void sendSensorValues() {
  getVibeValue();
}

void writeBuffer(int index, unsigned char c) {
  buf[index] = c;
}

void writeValue(unsigned char c) {
  BT.write(c);
}

void writeEnd() {
  BT.write(0xd);
  BT.write(0xa);
}

void sendFloat(float value) {
  val.floatVal = value;
  writeValue(val.byteVal[0]);
  writeValue(val.byteVal[1]);
  writeValue(val.byteVal[2]);
  writeValue(val.byteVal[3]);
}

void setup() {
  initPorts();
  BT.begin(115200);
}

void loop() {
  while (BT.available()) {
    unsigned char c = BT.read();
    updateDigitalPort(c);
  }

  if(rotation > 1000) {
    rotation = 0;
    sendSensorValues();
  }
  rotation++;
}

void callOK() {
  writeValue(0xff);
  writeValue(0xc);
  writeEnd();
}
