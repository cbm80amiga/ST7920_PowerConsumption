// ST7920 Power Consumption Tests
// (C)2019 Pawel A. Hernik

/*
 128x64 ST7920 connections (SPI)

 #01 GND  -> GND
 #02 VCC  -> VCC (5V)
 #04 RS   -> D10/CS or any pin
 #05 R/W  -> D11/MOSI
 #06 E    -> D13/SCK
 #15 PSB  -> GND (for SPI mode)
 #19 BLA  -> VCC or any pin via 300ohm
 #20 BLK  -> GND
*/

#define LCD_BACKLIGHT  9
#define LCD_CS         10

#include "ST7920_SPI.h"
#include <SPI.h>
ST7920_SPI lcd(LCD_CS);

// from PropFonts library
#include "small4x7_font.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(LCD_BACKLIGHT, OUTPUT);
  //analogWrite(LCD_BACKLIGHT,30);
  digitalWrite(LCD_BACKLIGHT, HIGH);

  //SPI.begin();
  lcd.init();
}

enum wdt_time {
  SLEEP_15MS,
  SLEEP_30MS, 
  SLEEP_60MS,
  SLEEP_120MS,
  SLEEP_250MS,
  SLEEP_500MS,
  SLEEP_1S,
  SLEEP_2S,
  SLEEP_4S,
  SLEEP_8S,
  SLEEP_FOREVER
};

ISR(WDT_vect) { wdt_disable(); }

void powerDown(uint8_t per)
{
  ADCSRA &= ~(1 << ADEN);  // turn off ADC
  if(per != SLEEP_FOREVER) { // use watchdog timer
    wdt_enable(per);
    WDTCSR |= (1 << WDIE);  
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // most power saving
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  // ... sleeping here
  sleep_disable();
  ADCSRA |= (1 << ADEN); // turn on ADC
} 

void wait()
{
  // force D13/SPI-SCK LED off and deep sleep for 4s
  SPI.end();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  powerDown(SLEEP_4S);
}

void loop() 
{
  int i;
  lcd.cls();
  lcd.setFont(Small4x7PL);
  lcd.printStr(ALIGN_CENTER, 28, "Starting ...");
  lcd.display(0);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  delay(3000);

  // backlight on
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight ON");
  lcd.display(0);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  wait();

  // backlight off
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight OFF");
  lcd.display(0);
  digitalWrite(LCD_BACKLIGHT, LOW);
  wait();

/*
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight ON");
  lcd.display(0);
  digitalWrite(LCD_BACKLIGHT, HIGH);
  delay(3000);
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight 192/255");
  lcd.display(0);
  analogWrite(LCD_BACKLIGHT, 192);
  delay(3000);
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight 128/255");
  lcd.display(0);
  analogWrite(LCD_BACKLIGHT, 128);
  delay(3000);
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight 64/255");
  lcd.display(0);
  analogWrite(LCD_BACKLIGHT, 64);
  delay(3000);
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight 32/255");
  lcd.display(0);
  analogWrite(LCD_BACKLIGHT, 32);
  delay(3000);
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Backlight OFF");
  lcd.display(0);
  digitalWrite(LCD_BACKLIGHT, LOW);
  delay(3000);
*/
  // 0xFF entire screen
  lcd.cls();
  lcd.fillRect(0,0,128,64,1);  // 0xFF
  lcd.display(0);
  wait();

  // 0xAA vertically
  for(i=0;i<128*64/8;i++) lcd.scr[i]=0xaa;
  lcd.display(0);
  wait();

  // 0xAA horizontally
  lcd.cls();
  for(i=0;i<64;i+=2) lcd.drawLineHfast(0,127,i,1); // 0xaa
  lcd.display(0);
  wait();

  // 0xF0 vertically
  for(i=0;i<128*64/8;i++) lcd.scr[i]=0xf0;
  lcd.display(0);
  wait();

  // 0xFO horizontally
  lcd.cls();
  for(i=0;i<8;i++) lcd.fillRect(0,i*8,128,4,1); // 0xf0
  lcd.display(0);
  wait();

  // 0x11 vertically
  for(i=0;i<128*64/8;i++) lcd.scr[i]=0x11;
  lcd.display(0);
  wait();

  // 0x11 horizontally
  lcd.cls();
  for(i=0;i<16;i++) lcd.drawLineHfast(0,127,i*4+3,1); // 0x11
  lcd.display(0);
  wait();

  // 0x01 vertically
  for(i=0;i<128*64/8;i++) lcd.scr[i]=0x01;
  lcd.display(0);
  wait();

  // 0xO1 horizontally
  lcd.cls();
  for(i=0;i<8;i++) lcd.drawLineHfast(0,127,i*8+7,1); // 0x01
  lcd.display(0);
  wait();

  // 0x00 entire screen
  for(i=0;i<128*64/8;i++) lcd.scr[i]=0x00;
  lcd.display(0);
  wait();

  // text
  lcd.printStr(0, 0*8, "Line 1");
  lcd.printStr(0, 1*8, "Line 2 Line 2");
  lcd.printStr(0, 2*8, "Line 3 Line 3 Line 3");
  lcd.printStr(0, 3*8, "Line 4 Line 4 Line 4 Line 4");
  lcd.printStr(0, 4*8, "Line 5 Line 5 Line 5 Line 5 Line 5");
  lcd.printStr(0, 5*8, "Line 6 Line 6 Line 6 Line 6 Line 6");
  lcd.printStr(0, 6*8, "Line 7 Line 7 Line 7 Line 7 Line 7");
  lcd.printStr(0, 7*8, "Line 8 Line 8 Line 8 Line 8 Line 8");
  lcd.display(0);
  wait();

  // text inverted
  lcd.fillRect(0,0,128,64,2);  // invert
  lcd.display(0);
  wait();

  // display off mode
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Display OFF in 2s ...");
  lcd.display(0);
  delay(2000);
  lcd.sendCmd(LCD_BASIC);
  lcd.sendCmd(B1000);
  wait();
  lcd.sendCmd(LCD_DISPLAYON);
  lcd.setGfxMode(true);
  //delay(del);

  // sleep mode
  lcd.cls();
  lcd.printStr(ALIGN_CENTER, 28, "Sleep mode in 2s ...");
  lcd.display(0);
  delay(2000);
  lcd.sleep(1);
  wait();
  lcd.sleep(0);
  //delay(del);

  // text mode
  lcd.setGfxMode(false);
  lcd.sendCmd(LCD_CLS);
  lcd.printTxt(LCD_LINE0,"Line 1");
  lcd.printTxt(LCD_LINE1,"Line 2 Line 2");
  lcd.printTxt(LCD_LINE2,"Line 3 Line 3");
  lcd.printTxt(LCD_LINE3,"Line 4 Line 4");
  wait();
  lcd.sendCmd(LCD_CLS); delay(2);
  //lcd.sendCmd(LCD_BASIC);
  lcd.setGfxMode(true);
  //lcd.sendCmd(LCD_BASIC);
}

