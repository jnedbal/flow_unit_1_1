/*******************/
/* Include headers */
/*******************/
// include the library for internal RTC
#include <rtc_clock.h>
// include the library to communicate with DS1307
#include <RTCdue.h>
// include the liquid crystal library code:
#include <LiquidCrystal.h>
// include the library for using timers.
#include <DueTimer.h>

/************************/
/* Initialize libraries */
/************************/

// Start the DS1338 RTC communication
RTC_DS1307 rtc;

// Start the internal RTC
RTC_clock rtc_clock(XTAL);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(14, 17, 15, 18, 16, 19);

/********************/
/* Define variables */
/********************/

// Days of the week
char* daynames[]={"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
// Error vector
byte err;
byte t;

// counting index
int i;

boolean ledOn = true;

// 34 byte long buffer storing serial data
byte serialbuffer[34];
// Serial FIFO index
byte Sin = 0;
// Serial transfer length (up to 32 bytes) plus 2.
// For transfer of 4 bytes, serlen should be 4+2=6.
byte serlen = 0;

/********************/
/* Define constants */
/********************/


void setup()
{
  // Start serial communication of programming port
  Serial.begin(115200);
  // Initialize the LCD
  initLCD();
  // Initialize the RTCs
  initRTC();
  // Attach an interrupt updating the clock
  Timer3.attachInterrupt(printTime).setFrequency(1).start();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  lcd.setCursor(0, 1);
  lcd.print(__TIME__);
  lcd.setCursor(0, 2);
  lcd.print(__DATE__);
}

void loop() {

  //printTime();
  rs232loop();
  //digitalWrite(13, digitalRead(13)?LOW:HIGH);
  //t++;
  //Serial.print(t, DEC);
  //Serial.print(" / 10 = ");
  //Serial.println(t / 10, DEC);
  //Serial.print(t, DEC);
  //Serial.print(" % 10 = ");
  //Serial.println(t % 10, DEC);
//  Serial.println(rtc_clock.current_time(), HEX);
//  Serial.print("At the third stroke, it will be ");
//  Serial.print(rtc_clock.get_hours());
//  Serial.print(":");
//  Serial.print(rtc_clock.get_minutes());
//  Serial.print(":");
//  Serial.println(rtc_clock.get_seconds());
//  Serial.print(daynames[rtc_clock.get_day_of_week()-1]);
//  Serial.print(": ");
//  Serial.print(rtc_clock.get_days());
//  Serial.print(".");
//  Serial.print(rtc_clock.get_months());
//  Serial.print(".");
//  Serial.println(rtc_clock.get_years());
  //delay(900);
}
