/*********************************************/
/* Functions dealing with errors of the unit */
/*********************************************/
//
// The error code is stored in an 8 bit variable err
// Each bit signalizes an error. If any error occurs
// It will be printed on the display in the top right
// corner (coordinate 17, 0).The error codes are 
// summarized below:
// 00000001:  External RTC was not running, power
//            failure. Update the time.
// 00000010:  External RTC encountered an oscillator
//            top. Some problem with power supply,
//            noise or board layout. Update the time.

void initError(void)
{
  Evector[15] = 0xFF;
  createErrorChar();
}

void createErrorChar(void)
{
  // create an inverse E symbol
  // #####  0x1F
  // #   #  0x11
  // # ###  0x17
  // #  ##  0x13
  // # ###  0x17
  // #   #  0x11
  // #####  0x1F
   uint8_t errorChar[8] = {0x1F, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1F};
   lcd.createChar(0, errorChar);
}

void displayError(void)
{
  if (err)
  {
    // If error has occurred, print it on the display
    lcd.setCursor(17, 0);
    lcd.write(byte(0));
    lcd.print(err, HEX);
  }
  else
  {
    lcd.setCursor(17, 0);
    lcd.print("   ");
  }
}
