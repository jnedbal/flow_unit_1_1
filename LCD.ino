/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 14
 * LCD Enable pin to digital pin 17
 * LCD D4 pin to digital pin 15
 * LCD D5 pin to digital pin 18
 * LCD D6 pin to digital pin 16
 * LCD D7 pin to digital pin 19
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

void initLCD(void)
{
  /* Set correct pins to outputs */
  pinMode(14, OUTPUT);    // LCD RS pin
  pinMode(15, OUTPUT);    // LCD Enable
  pinMode(16, OUTPUT);    // LCD D4
  pinMode(17, OUTPUT);    // LCD D5
  pinMode(18, OUTPUT);    // LCD D6
  pinMode(19, OUTPUT);    // LCD D7
  
  loadLCDdata(2, 58);   // Add a colon between hour and minute
  loadLCDdata(5, 58);   // Add a colon between minute and second
  loadLCDdata(11, 47);  // Add a slash between day and month
  loadLCDdata(14, 47);  // Add a slash between month and year
  loadLCDdata(40, 83);  // Add an "S" for (S)tir
  loadLCDdata(41, 116); // Add an "t" for S(t)ir
  loadLCDdata(42, 105); // Add an "i" for St(i)r
  loadLCDdata(43, 114); // Add an "r" for Sti(r)
  //loadLCDdata(45, 78);  // Add an "N" for (N)/A
  //loadLCDdata(46, 47);  // Add an "/" for N(/)A
  //loadLCDdata(47, 65);  // Add an "A" for N/(A)

  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
}

void printTime(void)
{
  //lcd.setCursor(0, 0);
  uint32_t curtime = rtc_clock.current_time();
  if (curtime == oldtime)
  {
    updateLCD();
    return;
  }
  oldtime = curtime;
  // Seconds
  loadLCDdata(0, HEXASCII[(curtime >> 20) & 0b11]);
  // Tens of seconds
  loadLCDdata(1, HEXASCII[(curtime >> 16) & 0b1111]);
  // Minutes
  loadLCDdata(3, HEXASCII[(curtime >> 12) & 0b111]);
  // Tens of minutes
  loadLCDdata(4, HEXASCII[(curtime >> 8) & B1111]);
  // Hours
  loadLCDdata(6, HEXASCII[(curtime >> 4) & B111]);
  // Tens of hours
  loadLCDdata(7, HEXASCII[(curtime) & B1111]);
  //lcd.print(" ");
  uint32_t curdate = rtc_clock.current_date();
  if (curdate == olddate)
  {
    return;
  }
  olddate = curdate;
  //lcd.print((curdate >> 28) & B11);
  loadLCDdata(9, HEXASCII[(curdate >> 28) & B11]);
  //lcd.print((curdate >> 24) & B1111);
  loadLCDdata(10, HEXASCII[(curdate >> 24) & B1111]);
  //lcd.print("/");
  //lcd.print((curdate >> 20) & B1);
  loadLCDdata(12, HEXASCII[(curdate >> 20) & B1]);
  //lcd.print((curdate >> 16) & B1111);
  loadLCDdata(13, HEXASCII[(curdate >> 16) & B1111]);
  //lcd.print("/");
  //lcd.print((curdate >> 12) & B1111);
  loadLCDdata(15, HEXASCII[(curdate >> 12) & B1111]);
  //lcd.print((curdate >> 8) & B1111);
  loadLCDdata(16, HEXASCII[(curdate >> 8) & B1111]);

  //lcd.setCursor(0, 0);
  //lcd.print(LCD0);
  

}

void updateLCD(void)
{
  if (LCDindex == 0)
  {
    return;
  }
  byte tmpIn = LCDindex - 1;
  lcd.setCursor(LCDnew[tmpIn] % 20, LCDnew[tmpIn] / 20);
  lcd.write(LCDdata[LCDnew[tmpIn]]);
  LCDindex--;
}

// Function updates LCDdata register, LCDnew
// register and increments LCDindex
void loadLCDdata(byte index, byte data)
{
//  if (LCDdata[index] == data)
//  {
//    return;
//  }
  LCDdata[index] = data;
  LCDnew[LCDindex] = index;
  LCDindex++;
}

