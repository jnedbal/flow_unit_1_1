// Section NVRAM deals with the non-volatile
// RAM 23LCV1024 backed-up with a battery.

// Each time an event occurs a series of information is stored.
// Bytes 0 - 1 are the event number
// Bytes 2 - 4 are the date
// Bytes 5 - 7 are the time
// Bytes 8 - 11 are milliseconds since the last restart
// Byte 12 events 1:
//  7: updated servo 1 (filter wheel 1)
//  6: updated servo 2 (filter wheel 2)
//  5: updated servo 3
//  4: updated servo 4
//  3: updated servo 1 look-up table
//  2: updated servo 2 look-up table
//  1: updated servo 3 look-up table
//  0: updated servo 4 look-up table
// Byte 13 events 2:
//  7:  reset
//  6:  error
//  5:  updated time
//  4:  updated backlight
//  3:  updated shutters
//  2:  reserved
//  1:  reserved
// Byte 14 events 3:
//  7:  reserved
//  6:  reserved
//  5:  reserved
//  4:  reserved
//  3:  reserved
//  2:  reserved
//  1:  reserved
// Byte 15 is the error vector
//  7:  External RTC was not running, power failure. Update the time.
//  6:  External RTC encountered an oscillator top. Some problem with
//            power supply, noise or board layout. Update the time.
//  5:
//  4:
//  3:
//  2:
//  1:
//  0:
// Byte 16 are servos 1 & 2 settings
//  7:  3 servo 2
//  6:  2 servo 2
//  5:  1 servo 2
//  4:  0 servo 2
//  3:  3 servo 1
//  2:  2 servo 1
//  1:  1 servo 1
//  0:  0 servo 1
// Byte 17 are servos 3 & 4 settings
//  7:  3 servo 4
//  6:  2 servo 4
//  5:  1 servo 4
//  4:  0 servo 4
//  3:  3 servo 3
//  2:  2 servo 3
//  1:  1 servo 3
//  0:  0 servo 3
// Byte 18 are shutter settings and LCD backlight
//  7:  LCD backlight ON
//  6:  shutter 4 ON
//  5:  shutter 3 ON
//  4:  shutter 2 ON
//  3:  shutter 1 ON
//  2:  reserved
//  1:  reserved
//  0:  reserved
// Byte 19 - 23 are reserved


void initNVRAM(void)
{
  initSPI(4, 21);
}

void testNVRAM(void)
{
  // Make sure the event address does not eveflow the 
  // 1 Mbit memory space. Update the event address.
  Eaddr = Eaddr + 0x10;
  if (Eaddr == 0x20000)
  {
    Eaddr = 0x100;
  }
  unsigned long addr = 0x01A6BA;
  SPI.transfer(4, writeInstr, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0x01, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0xA6, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0xBA, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0x34);
    SPI.transfer(4, readInstr, SPI_CONTINUE);
  SPI.transfer(4, (byte) ((addr >> 16) & 0xFF), SPI_CONTINUE);
  SPI.transfer(4, (byte) ((addr >> 8) & 0xFF), SPI_CONTINUE);
  SPI.transfer(4, (byte) (addr & 0xFF), SPI_CONTINUE);
  byte answer = SPI.transfer(4, 0x00);
  // had to comment out, otherwise the LCD buffer filled up too quickly
  //loadLCDdata(42, HEXASCII[(answer >> 4) & B1111]);
  //loadLCDdata(43, HEXASCII[(answer) & B1111]);
  //loadLCDdata(45, HEXASCII[(0x34 >> 4) & B1111]);
  //loadLCDdata(46, HEXASCII[(0x34) & B1111]);
  //lcd.setCursor(2, 2);
  //lcd.print((byte) answer, HEX);
  //lcd.setCursor(5, 2);
  //lcd.print((byte) 0x34, HEX);
  //lcd.setCursor(2, 3);
  //lcd.print((byte) ((addr >> 8) & 0xFF), HEX);
  delay(10);
}

void callEvent(void)
{
  // Make sure the event address does not overflow the 
  // 1 Mbit memory space. Update the event address.
  if (Eaddr > 0x20000)
  {
    Eaddr = 0x100;
  }

  // First two bytes contain the event number  
  Evector[0] = (byte) ((Ecount >> 8) & 0xFF);
  Evector[1] = (byte) (Ecount & 0xFF);
  // Next three bytes are the date
  uint32_t curtime = rtc_clock.current_date();
  Evector[2] = (byte) ((curtime >> 8) & 0xFF);
  Evector[3] = (byte) ((curtime >> 16) & 0xFF);
  Evector[4] = (byte) ((curtime >> 24) & 0xFF);
  // Next three bytes are the time
  curtime = rtc_clock.current_time();
  Evector[5] = (byte) ((curtime >> 16) & 0xFF);
  Evector[6] = (byte) ((curtime >> 8) & 0xFF);
  Evector[7] = (byte) (curtime & 0xFF);
  // Next four bytes are the milliseconds since last reset
  curtime = millis();
  Evector[8] = (byte) ((curtime >> 24) & 0xFF);
  Evector[9] = (byte) ((curtime >> 26) & 0xFF);
  Evector[10] = (byte) ((curtime >> 8) & 0xFF);;
  Evector[11] = (byte) (curtime & 0xFF);

  // Write instruction
  SPI.transfer(4, writeInstr, SPI_CONTINUE);
  // Pass on address
  SPI.transfer(4, (byte) ((Eaddr >> 16) & 0xFF), SPI_CONTINUE);
  SPI.transfer(4, (byte) ((Eaddr >> 8) & 0xFF), SPI_CONTINUE);
  SPI.transfer(4, (byte) (Eaddr & 0xFF), SPI_CONTINUE);
  for (i = 0; i++; i < 23)
  {
    SPI.transfer(4, Evector[i], SPI_CONTINUE);
  }
  SPI.transfer(4, Evector[23]);
  Eaddr = Eaddr + 0x18;
  Ecount = Ecount + 1;
}
