/*******************/
/* Include headers */
/*******************/
// include the library for internal RTC
#include <rtc_clock.h>
// include the library to communicate with DS1338
#include <RTClib.h>
// include the liquid crystal library code:
#include <LiquidCrystal.h>
// include the library for using timers.
#include <DueTimer.h>
// the NVRAM communicates using SPI, so include the library:
#include <SPI.h>
// I2C library is needed for communication with peripherals
#include <Wire.h>
// PCA9685 PWM driver for the backlight and contrast requires the library
#include <Adafruit_PWMServoDriver.h>
// Library to operate the servos
#include <Servo.h> 

/************************/
/*  #define Directives  */
/************************/


// Error vector as the 16th byte of the Event vector
#define err Evector[15]
// Event 1 vector as the 13th byte of the Event vector
#define ev1 Evector[12]
// Event 2 vector as the 14th byte of the Event vector
#define ev2 Evector[13]
// Servo 1&2 position
#define fw12 Evector[16]
// Servo 3&4 position
#define fw34 Evector[17]
// ON/OFF vector as the 19th byte of the Event vector
#define onoff Evector[18]
// Fluika pump pressure MSB
#define fp1 Evector[19]
// Fluika pump pressure LSB
#define fp2 Evector[20]

// Dividing factor for one ninth of the RAM full
#define oneNinth 0x38E4

// I2C address
//    Make sure this address is unique on the I2C bus and identical in the stirrer
#define SlaveDeviceId 9
// Number of bytes in I2C transfer to stirrer
#define lengthI2Cbuf 14

/************************/
/* Initialize libraries */
/************************/

// Start the DS1338 RTC communication
RTC_DS1307 rtc;

// Start the internal RTC
RTC_clock rtc_clock(XTAL);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(14, 17, 15, 18, 16, 19);

// initialize the library for the PWM driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x55);

// Initialize the servo library
Servo servoA;  // Servo 1
Servo servoB;  // Servo 2
Servo servoC;  // Servo 3
Servo servoD;  // Servo 4
Servo servos[] = {servoA, servoB, servoC, servoD};  // create servo object to control a servo 

/********************/
/* Define variables */
/********************/

// Identifier for MicroFLiC
char ID[] = "MicroFLiC v1.1 ArduinoDUE (Jan 16, 2015)";

// Days of the week
char* daynames[]={"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
// Old error vector
byte olderr;
byte t;

/* Variables for NVRAM */
// Event counter
unsigned long Ecount = 0;
// Event address
unsigned long Eaddr = 0x200;
// Event vector
byte Evector[24];
// NVRAM transfer buffer
byte NVbuffer[256];
// Memory fillup character
byte RAMfill[] = {0x4D, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xFF};



// LCD lines data storage
byte LCDdata[80];
byte LCDnew[80];
byte LCDindex;


uint32_t servIn;
uint16_t servPos;
// counting index
uint32_t i;

boolean ledOn = true;

// 34 byte long buffer storing serial data
byte serialbuffer[64];
// Serial FIFO index
byte Sin = 0;
// Serial transfer length (up to 62 bytes) plus 2.
// For transfer of 4 bytes, serlen should be 4+2=6.
byte serlen = 0;
// Serial packet index
byte packetIn = 0;

// *************************
// Filter position variables
// *************************
// Filter wheel address 
byte servoAddress[4];
// Number of active filter wheels
byte servoCount;
// Active filter wheels
byte servoActive[4];
// Number of characters in filter wheel names
byte servoNameNrChar[4];
// Filter wheel names
byte servoName[4][3];
// Filter wheel name LCD cursor position
byte servoNameLCD[] = {20, 0, 0, 0};


// Number of filters per servo
byte filterCount[4];
// Filter positions
uint16_t filterPosition[4][10];
// Maximum number of characters in filter names for each wheel
byte filterNameMaxChar[4];
// Number of characters in filter names for each wheel
byte filterNameNrChar[4][10];
// Filter names
byte filterName[4][10][2];
// Selected filter wheel
byte filterActive[4];
// Default filter wheel
byte filterDefault[4];
// Filter position name LCD cursor position
byte filterNameLCD[4];

// *************************
// Filter position variables
// *************************
byte readI2Cbuf[lengthI2Cbuf];

// **************
// Time variables
// **************
uint32_t oldtime;
uint32_t olddate;

/********************/
/* Define constants */
/********************/

/* Constants for NVRAM */
const byte writeInstr = 0x02;
const byte readInstr = 0x03;
/* End of constants for NVRAM */

/* Constants for LCD */
// lookup table for HEX numbers
const byte HEXASCII[16] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 59, 70};

void setup()
{
  // Start serial communication of programming port
  Serial.begin(115200);
  SerialUSB.begin(0);
  // Initialize the LCD
  initLCD();
  // Initialize error. Create an error character
  initError();


  // Initialize the RTCs
  initRTC();
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(13, LOW);
  // Initialize non-volatile RAM
  initNVRAM();
  delay(100);
  //testNVRAM();
  // Initialize the PWM
  initPWM();
  // Set 12 MHz clock for USB
  init12MHz();
  // Attach an interrupt updating the clock
  Timer1.attachInterrupt(printTime).setFrequency(50).start();
  // Set the servos
  initServo();
  // Initialize the stirrer
  readFromStirrer();
  // Call reset event
  ev2 |= 0b10000000;
  callEvent();
  
}

void loop() {

  //printTime();
  rs232loop();

  
  //testNVRAM();
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

  //delay(10);


//  RAMperc = Eaddr / 0x38E4;
//  if (RAMperc != RAMold)
//  {
//    RAMold = RAMperc;
//    lcd.setCursor(0, 3);
//    lcd.print(Eaddr, HEX);
//    lcd.print("  ");
//    lcd.print(RAMperc, HEX);
//    lcd.print("  ");
//    loadLCDdata(79, RAMfill[RAMperc]);
//    // Display the buffer onto the LCD
//    updateLCD();     
//  }
//
//    //
//
//
//
//  Eaddr += 0x18;  
//  if (Eaddr > 0x1FFFF)  // Don't overflow the memory
//  {
//    Eaddr = 0x200;
//    RAMperc = 0;
//  }
}
