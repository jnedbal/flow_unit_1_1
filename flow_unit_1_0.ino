/*******************/
/* Include headers */
/*******************/
// include the library for internal RTC
#include <rtc_clock.h>
// include the library to communicate with DS1307
#include <RTCdue.h>
// include the liquid crystal library code:
#include <LiquidCrystal.h>

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
/********************/
/* Define constants */
/********************/


void setup()
{
  Serial.begin(9600);
  initLCD();
  initRTC();
}

void loop() {

  printTime();
  //t++;
  //Serial.print(t, DEC);
  //Serial.print(" / 10 = ");
  //Serial.println(t / 10, DEC);
  //Serial.print(t, DEC);
  //Serial.print(" % 10 = ");
  //Serial.println(t % 10, DEC);
  Serial.println(rtc_clock.current_time(), HEX);
  Serial.print("At the third stroke, it will be ");
  Serial.print(rtc_clock.get_hours());
  Serial.print(":");
  Serial.print(rtc_clock.get_minutes());
  Serial.print(":");
  Serial.println(rtc_clock.get_seconds());
  Serial.print(daynames[rtc_clock.get_day_of_week()-1]);
  Serial.print(": ");
  Serial.print(rtc_clock.get_days());
  Serial.print(".");
  Serial.print(rtc_clock.get_months());
  Serial.print(".");
  Serial.println(rtc_clock.get_years());
  delay(900);
}
