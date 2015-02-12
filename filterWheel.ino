// Up to four filter wheels can be operated by servos.
// The NVRAM contains all the information to operate the servos.
// This script contains functions to load the information from the
// NVRAM to operate the servos correctly, control the servos and
// display the information on the LCD.

// Initialize the servos
void initServo(void)
{
  // Attach the servos running the filter wheels
  for (i = 0; i < servoCount; i++)
  {
    servos[i].attach(servoAddress[i]);
  }
  // Load the servos
  servoSetting();

}

void servoSetting(void)
{
  byte ind = 0;
  byte j = 0;
  // First we need to get information from the NVRAM. The NVRAM buffer is only
  // 256 bytes long, so we get it in two chunks.
  // The first 26 (0x1A) bytes starting at address 11 (0x0B)
  retrieveConstant(0x00, 0x00, 0x0B, 0x1A);

  // Start processing the data in the NVbuffer

  // Combine all bits of byte 8 to work out the number of filters
  // It is a four bit number 0000XXXX with the four bits (XXXX) 
  // telling whether a filter wheel is in use (1) or not in use (0)
  //   byte 9     list of active filter wheels

  for (i = 0; i < 4; i++)
  {
    // bit shift by value "i" and check if the last bit is one
    if ((NVbuffer[8] >> i) & 0x01)
    {
      // if condition met, add the address to the array
      servoActive[ind] = i;
      // increment counting index
      ind++;
    }
  }
  servoCount = ind;    // how many servos are used
  
  // Record the event


//  Serial.print("Active servo: ");
//  Serial.println(servoActive[0], DEC);
//  Serial.print("Active servo: ");
//  Serial.println(servoActive[1], DEC);
//  Serial.print("Active servo: ");
//  Serial.println(servoActive[2], DEC);
//  Serial.print("Active servo: ");
//  Serial.println(servoActive[3], DEC);
//  Serial.print("Number of active servos: ");
//  Serial.println(servoCount, DEC);

  // Two bytes for number of filter positions of filter wheels
  // BBBBAAAA, where BBBB is number of positions in wheel 2 and AAAA is number
  // of positions in wheel 1, DDDDCCCC, where DDDD is number of positions in
  // wheel 4 and CCCC is number of positions in wheel 3
  //   byte 23     number of positions in filter wheels 2 and 1
  //   byte 24     number of positions in filter wheels 4 and 3
  uint16_t numberPos = word(NVbuffer[23], NVbuffer[22]);

  // Two bytes for the default filter positions of filter wheels
  // BBBBAAAA, where BBBB is the default position in wheel 2 and AAAA is the
  // default position in wheel 1, DDDDCCCC, where DDDD is the default position
  // in wheel 4 and CCCC is the default position in wheel 3
  //   byte 25     default position in filter wheels 2 and 1
  //   byte 26     default position in filter wheels 4 and 3
  uint16_t defaultPos = word(NVbuffer[25], NVbuffer[24]);

  for (i = 0; i < servoCount; i++)
  {
    // Extract the address of filter wheels.
    //   byte 1      filter wheel 1 address MSB
    //   byte 2      filter wheel 1 address LSB
    //   byte 3      filter wheel 2 address MSB
    //   byte 4      filter wheel 2 address LSB
    //   byte 5      filter wheel 3 address MSB
    //   byte 6      filter wheel 3 address LSB
    //   byte 7      filter wheel 4 address MSB
    //   byte 8      filter wheel 4 address LSB
    servoAddress[i] = word(NVbuffer[servoActive[i] * 2], NVbuffer[servoActive[i] * 2 + 1]);
//    Serial.print("Filter address ");
//    Serial.print(servoActive[i] + 1, DEC);
//    Serial.print(": ");
//    Serial.println(servoAddress[i], DEC);

    // How many characters in each filter wheel name (up to 3) combined into one
    // byte (AABBCCDD), AA (filter wheel 4), BB (filter wheel 3), CC (filter
    // wheel 2), DD (filter wheel 1).
    //   byte 10     number of characters in filter wheel names
    // bitshift by value "i * 2" and get the two LSBs
    servoNameNrChar[i] = ((NVbuffer[9] >> (servoActive[i] * 2)) & 0x03);
//    Serial.print("Number of characters in servo ");
//    Serial.print(servoActive[i] + 1, DEC);
//    Serial.print(" name: ");
//    Serial.println(servoNameNrChar[i], DEC);

    // Get the names of the filter wheels
    // Three bytes for name of filter wheel 1
    //   byte 11     first byte of name of filter wheel 1
    //   byte 12     second byte of name of filter wheel 1
    //   byte 13     third byte of name of filter wheel 1
    // Three bytes for name of filter wheel 2
    //   byte 14     first byte of name of filter wheel 2
    //   byte 15     second byte of name of filter wheel 2
    //   byte 16     third byte of name of filter wheel 2
    // Three bytes for name of filter wheel 3
    //   byte 17     first byte of name of filter wheel 3
    //   byte 18     second byte of name of filter wheel 3
    //   byte 19     third byte of name of filter wheel 3
    // Three bytes for name of filter wheel 4
    //   byte 20     first byte of name of filter wheel 4
    //   byte 21     second byte of name of filter wheel 4
    //   byte 22     third byte of name of filter wheel 4
    for (j = 0; j < 3; j++)
    {
      servoName[i][j] = NVbuffer[servoActive[i] * 3 + 10 + j];
    }
//    Serial.print("Servo ");
//    Serial.print(servoActive[i] + 1, DEC);
//    Serial.print(" name: ");
//    Serial.print(servoName[i][0], DEC);
//    Serial.print(servoName[i][1], DEC);
//    Serial.println(servoName[i][2], DEC);

    // 16-bit word for number of filter positions of filter wheels
    // DDDDCCCCBBBBAAAA, where DDDD is number of positions in wheel 4,
    // CCCC is number of positions in wheel 3, BBBB is number of
    // positions in wheel 2 and AAAA is number of positions in wheel 1.
    filterCount[i] = ((numberPos >> (servoActive[i] * 4)) & 0x0F);
//    Serial.print("Number of filters in servo ");
//    Serial.print(servoActive[i] + 1, DEC);
//    Serial.print(": ");
//    Serial.println(filterCount[i], DEC);

    // 16-bit word for default filter position of filter wheels
    // DDDDCCCCBBBBAAAA, where DDDD is the dafault position in wheel 4,
    // CCCC is the default position in wheel 3, BBBB is the default
    // position in wheel 2 and AAAA is the default position in wheel 1.
    filterDefault[i] = ((defaultPos >> (servoActive[i] * 4)) & 0x0F);
//    Serial.print("Default filter in servo ");
//    Serial.print(servoActive[i] + 1, DEC);
//    Serial.print(": ");
//    Serial.println(filterDefault[i], DEC);
  }


  // 240 bytes of up to fourty filter positions and their names
  // Each filter position comes with 4 bytes. 
  // byte 1:   XX00YYYY, where XX is number of characters in filters name
  //                     where YY is four MSBs of a 12-bit number of filter
  //                     position ranging from 700 to 2300
  // byte 2:   ZZZZZZZZ, are the 8 LSBs of the filter position number
  // byte 3:   first character of the filter position name
  // byte 4:   second character of the filter position name
  // byte 5:   7 MSBs of 15-bit RGB code
  // byte 6:   8 LSBs of 15-bit RGB code
  //   bytes 27-266    filter positions and their names
  
  // The remaining 240 (0xF0) bytes starting at address 37 (0x25)
  retrieveConstant(0x00, 0x00, 0x25, 0xF0);

  for (i = 0; i < servoCount; i++)
  {
    // Maximum number of characters in filter position name
    filterNameMaxChar[i] = 0;
    for (j = 0; j < filterCount[i]; j++)
    {
      // index is the filter wheel number times ten, plus filter position
      // This is all multiplied by ten as there are 6 bytes per position
      ind = (servoActive[i] * 10 + j) * 6;
      // Get the number of characters in the filter name
      filterNameNrChar[i][j] = (NVbuffer[0 + ind] >> 6);
      filterNameMaxChar[i] = max(filterNameMaxChar[i], filterNameNrChar[i][j]);
      // Get the servo position for given filter
      filterPosition[i][j] = word(NVbuffer[0 + ind] & 0X0F, NVbuffer[1 + ind]);
      // Get the name of a filter position
      filterName[i][j][0] = NVbuffer[2 + ind];
      if (NVbuffer[3 + ind] == 0x00)
      {
        filterName[i][j][1] = 32;  // Put a space in instead
      }
      else
      {
        filterName[i][j][1] = NVbuffer[3 + ind];
      }
//      Serial.print("** Filter wheel: ");
//      Serial.print(servoActive[i] + 1, DEC);
//      Serial.print("; filter position: ");
//      Serial.print(j + 1, DEC);
//      Serial.println(" **");
//      Serial.print("Number of characters in a filter position: ");
//      Serial.println(filterNameNrChar[i][j], DEC);
//      Serial.print("Filter position: ");
//      Serial.println(filterPosition[i][j], DEC);
//      Serial.print("Filter name: ");
//      Serial.print(filterName[i][j][0], DEC);
//      Serial.println(filterName[i][j][1], DEC);
    }
//    Serial.print("Maximum number of characters in filter name of filter wheel ");
//    Serial.print(servoActive[i] + 1, DEC);
//    Serial.print(": ");
//    Serial.println(filterNameMaxChar[0], DEC);
  }


  // Work out the positions of the filter positions on the LCD
  // Filter wheel 1 position is always at the beginning of the line
  // Following filter wheel LCD cursor positions are always the previous position
  // plus the number of characters in that name and the maximum number of characters
  // in the wheel's filter position name
  for (i = 0; i < servoCount - 1; i++)
  {
    servoNameLCD[i + 1] = servoNameLCD[i] + servoNameNrChar[i] + filterNameMaxChar[i];
  }

  // Similarly, the filter position LCD cursor position is the position of the servo
  // name LCD cursor position and its length
  for (i = 0; i < servoCount; i++)
  {
    filterNameLCD[i] = servoNameLCD[i] + servoNameNrChar[i];
    // Put space characters on the display to delete anything that is left there
    for (byte j = filterNameLCD[i]; j < filterNameLCD[i] + filterNameMaxChar[i]; j++)
    {
      loadLCDdata(j, 32);
    }
  }

  // There might be something on the display at the end of the line
  // Replace anything after the last filter position name with spaces
  for (i = filterNameLCD[servoCount - 1] + filterNameMaxChar[servoCount - 1]; i < 40; i++)
  {
    loadLCDdata(i, 32);
  }

  // Load the filter wheel names on the LCD
  for (i = 0; i < servoCount; i++)
  {
    for (j = 0; j < servoNameNrChar[i]; j++)
    {
      loadLCDdata(servoNameLCD[i] + j, servoName[i][j]);
    }
  }

  // Set filter position error to zero
  // Will set it one if any filter is wrong
  err &= 0b11110111;
  
  // Check if there are any default filter positions to load
  for (i = 0; i < servoCount; i++)
  {
    // Check if the default filter is non-zero
    if (filterDefault[i] > 0)
    {
      // Move filter wheel to the default position
      servos[i].writeMicroseconds(filterPosition[i][filterDefault[i] - 1]);
      for (j = 0; j < filterNameMaxChar[i]; j++)
      {
        // Print characters on LCD
        loadLCDdata(filterNameLCD[i] + j, filterName[i][filterDefault[i] - 1][j]);
      }
      // Remember the active filter selection
      filterActive[i] = filterDefault[i];
    }
    else
    {
      // No filter selected
      filterActive[i] = 0x00;
      err |= 0b00001000;
    }
    displayError();
  }

  // Record the event
  // New filter positions
  fw12 = (filterActive[0] | (filterActive[1] << 4));
  fw34 = (filterActive[2] | (filterActive[3] << 4));
  callEvent();

  // Display the buffer onto the LCD
  updateLCD();
}

