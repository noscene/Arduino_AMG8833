/*
    - AMG8833 grideye from Adafruit
    - ESP32 (nodeMCU)
    - SSD_1332 OLED Color 0.96inch 96x64 pixel
    - based on Adafruit_AMG88xx library
*/

#include <SPI.h>
#include <SSD_13XX.h>
#include <Wire.h>

/*
  !!!!!!!!!!!! Update
  /Users/xxxxxxxxx/Documents/Arduino/libraries/SSD_13XX-master/xxx/SSD_13xx_settings.h
  #include "../_display/SSD_1332_96x64.h"
*/

#define __CS1 	2     // we dont use it in display (cs is hard wired to gnd, just for debug led stuff
#define __DC 	5
SSD_13XX tft = SSD_13XX(__CS1, __DC);

void setup() {

  tft.begin();
  tft.setBrightness(15);
  tft.clearScreen();
  tft.setRotation(0); // 0-4
  tft.setTextWrap(true);

  tft.setCursor(0, 0 );
  tft.setTextColor(GREEN);
  tft.setTextScale(2);
  tft.println("Start");

  amg8833_begin();

  tft.println(amg8833_readThermistor()); // amg.readThermistor()
  // default settings

  digitalWrite(2, false);
  delay(100); // let sensor boot up
  digitalWrite(2, true);
  delay(100); // let sensor boot up
  digitalWrite(2, false);
  delay(100); // let sensor boot up
  digitalWrite(2, true);
  delay(100); // let sensor boot up
  tft.clearScreen();

}

/***************************************************************/

#define AMG8833_I2C_ADDR              0x69
#define AMG8833_NORMAL_MODE           0x00
#define AMG8833_INITIAL_RESET         0x3F
#define AMG8833_FPS_10                0x00
#define AMG8833_INTC                  0x03
#define AMG8833_PCTL                  0x00
#define AMG8833_RST                   0x01
#define AMG8833_FPSC                  0x02
#define AMG8833_THERMISTOR_CONVERSION .0625
#define AMG8833_PIXEL_TEMP_CONVERSION .25
#define AMG8833_PIXEL_ARRAY_SIZE      64
#define AMG8833_PIXEL_OFFSET          0x80
#define AMG8833_TTHL                  0x0E

float pixels[AMG8833_PIXEL_ARRAY_SIZE];

void amg8833_setregister(uint8_t reg, uint8_t val){
  Wire.beginTransmission(AMG8833_I2C_ADDR);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)val);
  Wire.endTransmission();
}
uint16_t amg8833_read12bitword(uint8_t reg){
  Wire.beginTransmission(AMG8833_I2C_ADDR);
  Wire.write(reg );
  Wire.requestFrom(AMG8833_I2C_ADDR, (uint8_t )2);
  byte LSB = Wire.read();
  byte MSB = Wire.read();
  Wire.endTransmission();
  uint16_t val = ((MSB << 8) | LSB);
  return val;
}

void amg8833_begin() {
  Wire.begin();
  delay(10);  
  amg8833_setregister(AMG8833_RST, AMG8833_INITIAL_RESET);
  delay(200);
  amg8833_setregister(AMG8833_PCTL,AMG8833_NORMAL_MODE);
  delay(10);
  amg8833_setregister(AMG8833_FPSC,AMG8833_FPS_10);
  delay(20);
  amg8833_setregister(AMG8833_INTC,0);
  delay(20);
}

void amg8833_readPixels(float *buf) {
  for (int i = 0; i < 64; i++) {
    uint16_t val = amg8833_read12bitword(AMG8833_PIXEL_OFFSET + (i * 2) );
    uint16_t absVal = (val & 0x7FF);
    float converted =  (val & 0x8000) ? 0 - (float)absVal : (float)absVal ;
    buf[63 - i] = converted * AMG8833_PIXEL_TEMP_CONVERSION;
    Wire.endTransmission();
  }
}

float amg8833_readThermistor() {
  uint16_t val = amg8833_read12bitword(AMG8833_TTHL );
  uint16_t absVal = (val & 0x7FF);  
  float converted =  (val & 0x8000) ? 0 - (float)absVal : (float)absVal ;
  return  converted * AMG8833_THERMISTOR_CONVERSION;
}


/***************************************************************/



const uint16_t camColors[] = {0x480F,
                              0x400F, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810, 0x3010, 0x3010,
                              0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010, 0x2010, 0x1810, 0x1810,
                              0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811, 0x0811, 0x0811, 0x0011, 0x0011,
                              0x0011, 0x0011, 0x0011, 0x0031, 0x0031, 0x0051, 0x0072, 0x0072, 0x0092, 0x00B2,
                              0x00B2, 0x00D2, 0x00F2, 0x00F2, 0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192,
                              0x0192, 0x01B2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273,
                              0x0293, 0x02B3, 0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373,
                              0x0394, 0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
                              0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554, 0x0574,
                              0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571, 0x0591, 0x0591,
                              0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE, 0x05AE, 0x05AD, 0x05AD,
                              0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB, 0x05CA, 0x05CA, 0x05CA, 0x05C9,
                              0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7, 0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5,
                              0x05E5, 0x0604, 0x0604, 0x0604, 0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621,
                              0x0621, 0x0620, 0x0620, 0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640,
                              0x1E40, 0x1E40, 0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60,
                              0x3E60, 0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
                              0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0, 0x8EA0,
                              0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0, 0xB6E0, 0xB6E0,
                              0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0, 0xD700, 0xDF00, 0xDEE0,
                              0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640, 0xE620, 0xE600, 0xE5E0, 0xE5C0,
                              0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520, 0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480,
                              0xE460, 0xEC40, 0xEC20, 0xEC00, 0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40,
                              0xEB20, 0xEB00, 0xEAE0, 0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200,
                              0xF1E0, 0xF1C0, 0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0,
                              0xF080, 0xF060, 0xF040, 0xF020, 0xF800,
                             };


#define cam_area_width 12
#define cam_area_height 12

//low range of the sensor (this will be blue on the screen)
#define MINTEMP 19

//high range of the sensor (this will be red on the screen)
#define MAXTEMP 36

void loop(void) {
  //tft.clearScreen();
  amg8833_readPixels(pixels);

  for (int i = 0; i < AMG8833_PIXEL_ARRAY_SIZE; i++) {
    uint8_t colorIndex = map(pixels[i], MINTEMP, MAXTEMP, 0, 255);
    colorIndex = constrain(colorIndex, 0, 255);
    //draw the pixels!
    tft.fillRect(cam_area_height * floor(i / 8), cam_area_width * (i % 8) ,
                 cam_area_height, cam_area_width, camColors[colorIndex]);
  }
  /*
    tft.setCursor(0, 0 );
    tft.setTextColor(GREEN);
    tft.setTextScale(2);
    tft.println("AMG8833");
  */
  tft.setCursor(8, 8 );
  tft.setTextColor(WHITE);
  tft.setTextScale(0);
  tft.println(amg8833_readThermistor()); 
  digitalWrite(2, false);
  delay(80);
}

