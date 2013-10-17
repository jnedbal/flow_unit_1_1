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
// the NVRAM communicates using SPI, so include the library:
#include <SPI.h>


/************************/
/*  #define Directives  */
/************************/

// Error vector as the 16th byte of the Event vector
#define err Evector[15]
// Event 1 vector as the 13th byte of the Event vector
#define ev1 Evector[12]
// Event 2 vector as the 14th byte of the Event vector
#define ev2 Evector[13]

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
// Old error vector
byte olderr;
byte t;

/* Variables for NVRAM */
// Event counter
unsigned long Ecount = 0;
// Event address
unsigned long Eaddr = 0x100;
// Event vector
byte Evector[24];

// LCD lines data storage
//byte LCDdata[80];
//byte LCDnew[80];
//byte LCDindex;


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

/* Constants for NVRAM */
const byte writeInstr = 0x02;
const byte readInstr = 0x03;
/* End of constants for NVRAM */

void setup()
{
  // Initialize error. Create an error character
  initError();
  // Start serial communication of programming port
  Serial.begin(115200);
  // Initialize the LCD
  initLCD();
  // Initialize the RTCs
  initRTC();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  // Initialize non-volatile RAM
  initNVRAM();
  delay(100);
  testNVRAM();
  // Attach an interrupt updating the clock
  Timer3.attachInterrupt(printTime).setFrequency(1).start();
  // Call reset event
  ev2 = ev2 | B10000000;
  callEvent();
  ev2 = ev2 & B01111111;
}

void loop() {

  //printTime();
  rs232loop();
  testNVRAM();
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
