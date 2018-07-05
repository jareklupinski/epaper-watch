#include <RTCZero.h>
#include <UC8173.h>
#include <MatrixMath.h>
#include "assets/face1.c"
#include "assets/face2.c"
#include "assets/face3.c"
#include "assets/minuteHand.c" // minute hand is 40 x 120
#include "assets/hourHand.c" // minute hand is 40 x 80

#define SDA_PIN ( 1 << 22 ) //5
#define SCL_PIN ( 1 << 23 ) //6
#define CS_PIN ( 1 << 7 )   //9
#define DC_PIN ( 1 << 18 )  //10
#define RST_PIN ( 1 << 16 ) //11
#define BSY_PIN ( 1 << 19 ) //12
#define BS_PIN ( 1 << 17 )  //13

const float pi = 3.14;
const byte seconds = 0;
const byte minutes = 35;
const byte hours = 15;
const byte day = 15;
const byte month = 11;
const byte year = 16;
RTCZero rtc;
UC8173 epaper( SDA_PIN, SCL_PIN, CS_PIN, DC_PIN, RST_PIN, BS_PIN, BSY_PIN );

void rotatePixel( float theta, uint16_t x, uint16_t y, int color ) {
  float A[2][1];
  float B[2][2];
  float C[2][1];
  A[0][0] = (float)x - 120.0f;
  A[0][1] = (float)y - 120.0f;
  B[0][0] = cos( theta );
  B[0][1] = sin( theta );
  B[1][0] = sin( theta ) * -1.0f;
  B[1][1] = cos( theta );
  Matrix.Multiply( (float*)A, (float*)B, 1, 2, 2, (float*)C );
  uint16_t d = (uint16_t)C[0][0] + 120;
  uint16_t e = (uint16_t)C[0][1] + 120;
  epaper.drawPixel( d, e, color );
}

void drawSmoothHand( int s ) {
  int16_t i, j, byteWidth = (40 + 7) / 8;
  uint8_t b;
  for (j = 0; j < 120; j++) {
    for (i = 0; i < 40; i++ ) {
      if (i & 7) b <<= 1;
      else b   = minuteHand[j * byteWidth + i / 8];
      if (b & 0x80) rotatePixel( (((((float)s + 30 ) * 1.0f) / 360.0f) * (2.0f * pi)), i + 100, j + 120, 1 );
    }
  }
}

void drawMinuteHand( int m ) {
  int16_t i, j, byteWidth = (40 + 7) / 8;
  uint8_t b;
  for (j = 0; j < 120; j++) {
    for (i = 0; i < 40; i++ ) {
      if (i & 7) b <<= 1;
      else b   = minuteHand[j * byteWidth + i / 8];
      if (b & 0x80) rotatePixel( (((((float)m + 30 ) * 6.0f) / 360.0f) * (2.0f * pi)), i + 100, j + 120, 1 );
    }
  }
}

void drawHourHand( int h ) {
  int16_t i, j, byteWidth = (40 + 7) / 8;
  uint8_t b;
  for (j = 0; j < 80; j++) {
    for (i = 0; i < 40; i++ ) {
      if (i & 7) b <<= 1;
      else b   = hourHand[j * byteWidth + i / 8];
      if (b & 0x80) rotatePixel( (((((float)(h * 5) + 30 ) * 6.0f) / 360.0f) * (2.0f * pi)), i + 100, j + 120, 1 );
      //else rotatePixel( (((((float)(h * 5) + 30 ) * 6.0f) / 360.0f) * (2.0f * pi)), i + 100, j + 120, 0 );
    }
  }
}

void drawFace() {
  int16_t i, j, byteWidth = (240 + 7) / 8;
  uint8_t b;
  for (j = 0; j < 240; j++) {
    for (i = 0; i < 240; i++ ) {
      if (i & 7) b <<= 1;
      else b   = face1 [j * byteWidth + i / 8];
      if (b & 0x80) epaper.drawPixel( i, j, 1 );
      else epaper.drawPixel( i, j, 0 );
    }
  }
}

void setup() {
  Serial.begin(115200);
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
  delay( 2000 );
  pinMode( A0, OUTPUT );
  digitalWrite( A0, HIGH );
  Serial.println( "starting" );
  epaper.EPD_Init();
}

void loop() {
  drawFace();
  drawMinuteHand( rtc.getSeconds() );
  drawHourHand( rtc.getHours() % 12 );
  epaper.EPD_Draw();
  Serial.println( "drawing" );
  //epaper.EPD_Display_Update();
  /*
    rtc.setAlarmSeconds((rtc.getSeconds() + 2) % 60); // RTC time to wake, currently seconds only
    rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
    rtc.standbyMode();    // Sleep until next alarm match
  */
}
