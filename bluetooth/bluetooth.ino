#include <SoftwareSerial.h>
#include <LedControl.h>
#define bt_tx 2
#define bt_rx 3
#define sound 13
SoftwareSerial BTSerial(2, 3);
byte buffer[1024];
int bufferPosition;

LedControl lc = LedControl(12, 11, 10, 1);// 12(DIN), 11(CLK), 10(CS)핀을 사용, 1개의 도트매트릭스 제어하는 객체 생성
extern volatile unsigned long timer0_millis;

byte smile[8] = { B00000000, B01100110, B10011001, B00000000, B00000000, B01000010, B00111100, B00000000};// 웃는얼굴을 표시할 수 있는 배열을 생성
byte annoying[8] = { B10000001, B01000010, B11100111, B01000010, B10000001, B00011000, B00100100, B01000010};// 화난얼굴을 표시할 수 있는 배열을 생성
byte crying[8] = { B00000000, B00000000, B01100110, B10000001, B00000000, B00011000, B00100100, B00000000};// 우는얼굴을 표시할 수 있는 배열을 생성
int cnt = 0;
int btn = 9;
int data = 0;

void buzzeron() {
  tone(sound, 3000, 1000);
}
void buzzeroff() {
  digitalWrite(sound, 0);
}
void non() {
  lc.clearDisplay(0);
}
void smile1() {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, smile[row]);
    delay(25);
  } //스마일끝
  delay(10000);
}
void annoying1() {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, annoying[row]);
    delay(25);
  }
} //어노잉끝*/

void crying1() {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, crying[row]);
    delay(25);
  }
  for (int col = 3; col < 6; col++) {
    for (int row = 1; row < 2; row++) {
      lc.setLed(0, col, row, true);
      delay(50);
    }
  }
  for (int col = 3; col < 6; col++) {
    for (int row = 1; row < 2; row++) {
      lc.setLed(0, col, row, false);
      delay(50);
    }
  }
  for (int col = 3; col < 7; col++) {
    for (int row = 6; row < 7; row++) {
      lc.setLed(0, col, row, true);
      delay(50);
    }
  }
  for (int col = 3; col < 7; col++) {
    for (int row = 6; row < 7; row++) {
      lc.setLed(0, col, row, false);
      delay(50);
    }
  }
}//크라잉 끝

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(9, INPUT_PULLUP);
  pinMode(sound, OUTPUT);
  bufferPosition = 0;


  lc.shutdown(0, false); // 도트매트릭스의 절전모드 끄기
  lc.setIntensity(0, 8); // 도트매트릭스의 LED 밝기를 8로 지정(0~15)
  lc.clearDisplay(0); // 도트매트릭스의 LED 초기화
}

void loop() {
  int key_in;
  unsigned long time1 = millis() / 1000;
  Serial.println(time1);// 시간초 타이머
  Serial.print("count : ");
  Serial.println(cnt);
  Serial.print("data : ");
  Serial.println(data);
  delay(400);

  if (time1 >= 5 && time1 <= 15) { //5~15초동안 data값이 1일때
    buzzeron();
    annoying1();
    if (data == 1) {
      buzzeroff();
      BTSerial.println('q');
      smile1(); // 스마일 출력
      non(); //3초가 지나면 화면이 꺼짐

    }
  }
  if (time1 >= 16 && time1 <= 25) { //16~25초동안 data값이 0일때
    if (data == 0) {
      buzzeroff();
      for (int sig = 0; sig < 3; sig++) { // 크라잉을 세번 반복하기 위해
        crying1(); // 크라잉 출력!!(현재 문제점 크라잉이 세번 수행되고 25초가 안지나서 한번 더 수행됨)
      }
      non();//
    }
  }
  if (time1 >= 26 && time1 < 35) {
    if (data == 1) {
      data = 0;
    }
  }
  if (time1 >= 36 && time1 <= 45) {
    annoying1();
    buzzeron();
    if (data == 1) {
      buzzeroff();
      BTSerial.println('w');
      smile1();
      non();
    }
  }
  if (time1 >= 46 && time1 <= 55) {
    if (data == 0) {
      for (int sig = 0; sig < 3; sig++) {
        buzzeroff();
        crying1();
        delay(10000);
      }
      non();
    }
  }
  if (time1 >= 56 && time1 < 65) {
    if (data == 1) {
      data = 0;
    }
  }
  if (time1 >= 66 && time1 <= 75) {
    annoying1();
    buzzeron();
    if (data == 1) {
      buzzeroff();
      BTSerial.println('e');
      smile1();
      non();
    }
  }
  if (time1 >= 76 && time1 <= 85) {
    if (data == 0) {
      buzzeroff();
      for (int sig = 0; sig < 3; sig++) {
        crying1();
      }
      non();
    }
  }
  if (time1 >= 86 && time1 < 95) {
    if (data == 1) {
      data = 0;
    }
  }
  ////////////////버튼영역///////////////////

  if (digitalRead(btn) == HIGH) {
    if (cnt < 3) {
      cnt += 1;//누른횟수 카운트를 +1
      BTSerial.println(cnt);
      if (!key_in) {
        key_in = 1;
        data = !data;
      }
      delay(300);
    }
  }
  else key_in = 0;


  if (BTSerial.available()) {
    // 수신 받은 데이터 저장
    byte data1 = BTSerial.read();
    // 수신된 데이터 시리얼 모니터로 출력
    Serial.write(data1);
    // 수신 받은 데이터를 버퍼에 저장
    buffer[bufferPosition++] = data1;

    // 문자열 종료 표시
    if (data1 == '\n') {
      buffer[bufferPosition] = '\0';
      cnt = 0;
      // 스마트폰으로 문자열 전송
      BTSerial.write(buffer, bufferPosition);
      bufferPosition = 0;
    }
  }
}

////////////////버튼영역///////////////////
