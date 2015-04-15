#define pumpAddress 0x0B

void checkSum(void)
{
  /* Checksum from
     https://github.com/alvieboy/arduino-oscope/blob/master/oscope.pde
     It makes bitwise XOR on the entire stack of data */
  byte cksum = 0;
  for (i = 0; i < Sin; i++)
  {
    cksum ^= serialbuffer[i];
  }
  SerialUSB.write(cksum);
  /* Octave code to calculate the checksum
  A=[repmat('0', 5, 1) dec2bin(double('Dqwer'))]
  W='        ';V=repmat(false,1,8);u=0;for i = 1:8;for j=1:5; u=u+1;V(i)=xor(V(i), logical(bin2dec(A(u))));end;end;for i=1:8; W(i)=dec2bin(V(i));end;W=bin2dec(W);char(W)
  */
}

void rs232loop(void)
{
  /*******************************************************/
  /* This routine should run in loop all the time.       */
  /* It listens to the serial port for instructions.     */
  /* The first byte it receives gives the number of      */
  /* bytes in the transmission packet.                   */
  /* The second byte is the instruction.                 */
  /* The remaining bytes are the data                    */
  /*******************************************************/

  if (!SerialUSB.available() > 0)
  {
    return;
  }
  // Get incoming byte and store it in the serial buffer.
  serialbuffer[Sin] = SerialUSB.read();
  // Increment the serial buffer index.
  Sin++;
  // Read the very first byte of the serial buffer.
  // It contains the length of the data transfer "serlen".
  if (Sin == 1)
  {
    serlen = serialbuffer[0];
  }
  // Once the entire length of the packet has been received, process it.
  if (Sin == serlen)
  {
    switch (serialbuffer[1])      // the second byte contains the command
    {
      /* if the command is G (71 in ASCII) for Get time */
      case 71:
        getTimeDate();
        break;

      /* if the command is I (73 in ASCII) for ID */
      case 73:
        getID();
        break;

      /* if the command is L (76 in ASCII) for LCD */
      case 76:
        getLCDcontrastBrightness();
        break;

      /* if the command is C (67 in ASCII) for LCD */
      case 67:
        setLCDcontrastBrightness();
        break;

      /* if the command is S (83 in ASCII) for Set time */
      case 83:
        setTimeDate();
        break;

      /* if the command is p (112 in ASCII) to store NVRAM packet */
      case 112:
        packetStore();
        break;

      /* if the command is P (80 in ASCII) to send NVRAM Packet */
      case 80:
        packetSend();
        break;

      /* Reset the filter lookup table l (108 in ASCII) */
      case 108:
        filterUpdate();
        break;

      /* if the command is f (102 in ASCII) to set filter position */
      case 102:
        filterSet();
        break;

      /* if the command is F (70 in ASCII) to get filter position */
      case 70:
        filterGet();
        break;

      /* if the command is g (103 in ASCII) to goto filter position */
      case 103:
        filterGoto();
        break;

      /* if the command is E (69 in ASCII) to get eventNR */
      case 69:
        EcountGet();
        break;

      /* if the command is b (98 in ASCII) to bulk transfer all events */
      case 98:
        transferEvents();
        break;

      /* if the command is e (101 in ASCII) to (e)rase Event memory */
      case 101:
        wipeEvents();
        break;

      /* if the command is D (68 in ASCII) for (D)olomite pump ID*/
      case 68:
        pumpID();
        break;

      /* if the command is d (100 in ASCII) for (d)olomite pump pressure */
      case 100:
        pumpPressure();
        break;

      /* if the command is s (115 in ASCII) for (s)tirrer */
      case 115:
        setStirrer();
        break;

      /* if the command is t (116 in ASCII) for s(t)irrer */
      case 116:
        getStirrer();
        break;

    }
  }
}

void setTimeDate()
{
  // Arrays to hold strings of time and date
  char ctime[8];     // time (HH:MM:SS)
  char cdate[11];   // date (mmm dd yyyy)
  // load bytes 3 to 9 containing current time into ctime
  for(i = 2; i < 10; i++)
  {
    ctime[i - 2] = (char) serialbuffer[i];
  }
  // load bytes 10 to 20 containing current date into cdate
  for(i = 10; i < serlen; i++)  // loop through bytes 3 to 9 of the array
  {
    cdate[i - 10] = (char) serialbuffer[i];
  }
  // Update the external RTC
  rtc.adjust(DateTime(cdate, ctime));
  // Recall the time from the external RTC
  DateTime now = rtc.now();
  // Update time and date of internal RTC
  rtc_clock.set_time(now.hour(), now.minute(), now.second());
  rtc_clock.set_date(now.day(), now.month(), now.year());
  // Update the oscillator stop flag to 0.
  // It requires setting bit 5 in register 0x07 to zero.
  rtc.writenvram(0x07, rtc.readnvram(0x07) & B11011111);
  // Make sure external RTC is not giving errors
  RTCerrorCheck();

  // Update the event register 2 to say that time has changed
  ev2 |= 0b00100000;
  // Store event
  callEvent();
  // Update the event register to say that time NOT changed anymore
  ev2 &= 0b11011111;

  // Return checksum
  checkSum();
  Sin = 0;
  serlen = 0;
}

void getTimeDate(void)
{
  uint32_t curtime = rtc_clock.current_time();
  uint32_t curdate = rtc_clock.current_date();
  uint8_t buffer[6];
  buffer[0] = (curtime >> 16) & 0xFF;
  buffer[1] = (curtime >> 8) & 0xFF;
  buffer[2] = curtime & 0xFF;
  buffer[3] = (curdate >> 24) & 0xFF;
  buffer[4] = (curdate >> 16) & 0xFF;
  buffer[5] = (curdate >> 8) & 0xFF;

  // Return checksum
  checkSum();
  Sin = 0;
  serlen = 0;

  SerialUSB.write(buffer, 6);
}

void getID(void)
{
  // Return checksum
  checkSum();
  Sin = 0;
  serlen = 0;

  SerialUSB.write(sizeof(ID));
  SerialUSB.write(sizeof(__DATE__));
  SerialUSB.write(sizeof(__TIME__));
  SerialUSB.print(ID);
  SerialUSB.print(__DATE__);
  SerialUSB.print(__TIME__);
}

void getLCDcontrastBrightness(void)
{
  retrieveConstant(0x00, 0x00, 0x07, 0x04);

  // Return checksum
  checkSum();
  Sin = 0;
  serlen = 0;

  SerialUSB.write(NVbuffer, 4);
}

void setLCDcontrastBrightness(void)
{
  // Store last four bytes into NVRAM buffer
  for (i = 0; i < 4; i++)
  {
    NVbuffer[i] = serialbuffer[i + 2];
  }
  // Save four bytes into address 0x07 in the NVRAM
  storeConstant(0x00, 0x00, 0x07, 0x04);
  // Set the brightness
  setLCDbrightness(word(NVbuffer[0], NVbuffer[1]));
  // Set the contrast
  setLCDcontrast(word(NVbuffer[2], NVbuffer[3]));

  // Update the event register
  ev2 |= 0b00010000;
  callEvent();

  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to store packets with filter wheel settings
void packetStore(void)
{
  // Data comes in four packet of maximum 64 bytes, they need
  // to be stored into NVbuffer and stored into the NVRAM

  // store up to 60 bytes into NVbuffer
  for (i = 4; i < serialbuffer[0]; i++)
  {
    NVbuffer[i - 4] = serialbuffer[i];
  }

  // Store into NVRAM
  storeConstant(0x00, serialbuffer[2], serialbuffer[3], serialbuffer[0] - 4);

  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to send a packet with filter wheel settings
void packetSend(void)
{
  retrieveConstant(0x00, serialbuffer[2], serialbuffer[3], serialbuffer[4]);
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
  SerialUSB.write(NVbuffer, serialbuffer[4]);
}

// Function to send a information about selected filter
void filterGet(void)
{
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
  //SerialUSB.write(servoCount);
  // Create a 4 byte array to hold the active filter positions
  byte sendBuffer[4];
  for (i = 0; i < servoCount; i++)
  {
    sendBuffer[servoActive[i]] = filterActive[i];
  }
  // Send to the PC
  SerialUSB.write(sendBuffer, 4);
//  SerialUSB.write(servoActive, 4);
//  SerialUSB.write(filterActive, 4);
//  SerialUSB.write(filterDefault, 4);
//  SerialUSB.write(filterActive[0]);
//  SerialUSB.write(servoActive[0]);
//  SerialUSB.write(sendBuffer[0]);
}

// Function to set a filter in a particular position
void filterSet(void)
{
  // serialbuffer[2]:  which wheel is used
  // serialbuffer[3]:  which position to go to
  // Go to the position
  //servos[serialbuffer[2]].writeMicroseconds(filterPosition[serialbuffer[2]][serialbuffer[3] - 1]);
  moveServo(servoAddress[serialbuffer[2]], filterPosition[serialbuffer[2]][serialbuffer[3] - 1]);
  for (i = 0; i < filterNameMaxChar[serialbuffer[2]]; i++)
  {
    // Print characters on LCD
    loadLCDdata(filterNameLCD[serialbuffer[2]] + i, filterName[serialbuffer[2]][serialbuffer[3] - 1][i]);
  }
  // Set the active filter position
  filterActive[serialbuffer[2]] = serialbuffer[3];
  if (err == 0b00001000)
  {
    err &= 0b11110111;
    for (i = 0; i < servoCount; i++)
    {
      if (filterActive[i] == 0)
      {
        err |= 0b00001000;
        ev2 |= 0b01000000;
      }
    }
    displayError();
  }

  // Update the log
  if (serialbuffer[2] < 2)
  {
    fw12 = (filterActive[0] | (filterActive[1] << 4));
  }
  else
  {
    fw34 = (filterActive[2] | (filterActive[3] << 4));
  }
  callEvent();
  
  // Display the buffer onto the LCD
  updateLCD();

  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to update the filter look-up-table
void filterUpdate(void)
{
  servoSetting();
  // Update filter look up table
  ev1 = ((ev1 & 0b11110000) | NVbuffer[8]);
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to set a filter in a particular position
void filterGoto(void)
{
  // serialbuffer[2]:    which wheel is used
  // serialbuffer[3-4]:  which position to go to, consists of a 16bit number between 700 and 2300
  // Go to the position
  // Switch on the servo only temporarily to prevent jumping of the servo
  //if (!servos[serialbuffer[2]].attached())
  {
    //servos[serialbuffer[2]].attach(servoAddress[serialbuffer[2]]);
  }
  //servos[serialbuffer[2]].writeMicroseconds(word(serialbuffer[3], serialbuffer[4]));
  moveServo(servoAddress[serialbuffer[2]], word(serialbuffer[3], serialbuffer[4]));

  // Delete filter position name from the LCD
  for (i = 0; i < filterNameMaxChar[serialbuffer[2]]; i++)
  {
    // Print characters on LCD
    loadLCDdata(filterNameLCD[serialbuffer[2]] + i, 32);
  }

  // Set the active filter position
  filterActive[serialbuffer[2]] = 0;
  // Set error on the display as we are moving to arbitrary position
  err |= 0b00001000;
  ev2 |= 0b01000000;

  displayError();

  // Update the log
  if (serialbuffer[2] < 2)
  {
    fw12 = (filterActive[0] | (filterActive[1] << 4));
  }
  else
  {
    fw34 = (filterActive[2] | (filterActive[3] << 4));
  }
  callEvent();

  //delay(1000);
  //servos[serialbuffer[2]].detach();
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to send a information about selected filter
void EcountGet(void)
{
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
  // Send to the PC
  SerialUSB.write((byte) (Ecount >> 8) & 0xFF);
  SerialUSB.write((byte) Ecount & 0xFF);
}

// Function to send all events in the NVRAM
void transferEvents(void)
{
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
  // Do the bulk transfer
    // Write instruction
  SPI.transfer(4, readInstr, SPI_CONTINUE);
  // Pass on address
  SPI.transfer(4, 0x00, SPI_CONTINUE);
  SPI.transfer(4, 0x00, SPI_CONTINUE);
  SPI.transfer(4, 0x00, SPI_CONTINUE);
  // Pass on the data byte-by-byte, don't transfer the last byte
  for (i = 0x01; i < Eaddr; i++)
  {
    SerialUSB.write(SPI.transfer(4, 0x00, SPI_CONTINUE));
  }
  SerialUSB.write(SPI.transfer(4, 0x00));
}

// Function to send all events in the NVRAM
void wipeEvents(void)
{
  // Reset the Ecount index and the event memory
  // Event counter
  Ecount = 0;
  // Event address
  Eaddr = 0x200;
  // Write zeros to the whole of the Event space of the memory
  // Write instruction
  SPI.transfer(4, writeInstr, SPI_CONTINUE);
  // Pass on address
  SPI.transfer(4, 0x00, SPI_CONTINUE);
  SPI.transfer(4, 0x02, SPI_CONTINUE);
  SPI.transfer(4, 0x00, SPI_CONTINUE);
  for (i = 0x200; i < 0x1FFFF; i++)
  {
    SPI.transfer(4, 0x00, SPI_CONTINUE);
  }
  SPI.transfer(4, 0x00);
  // Call an event to get something into the memory
  callEvent();
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to store the Device name and serial number into the NVRAM
void pumpID(void)
{
  // Transfer the 7 bytes to NVbuffer
  for (i = 0; i < 7; i++)
  {
    NVbuffer[i] = serialbuffer[i + 2];
  }
  storeConstant(0x00, 0x00, pumpAddress, 0x07);
  // Create a pump event
  ev2 |= 0b00000100;
  callEvent();
  // Display 'p=' on the screen
  loadLCDdata(40, 112);
  loadLCDdata(41, 61);
  // Display '000' on the screen
  loadLCDdata(42, 48);
  loadLCDdata(43, 48);
  loadLCDdata(44, 48);
  // Display 'mb' on the screen
  loadLCDdata(45, 109);
  loadLCDdata(46, 98);

  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}

// Function to store the pump pressure into the NVRAM
void pumpPressure(void)
{
  // Create a pump pressure event
  ev2 |= 0b00000100;
  fp1 = serialbuffer[2];
  fp2 = serialbuffer[3];
  callEvent();
  uint16_t intNumber = word(serialbuffer[2], serialbuffer[3]);
  for (i = 0; i < 3; i++)
  {
    loadLCDdata(44 - i, HEXASCII[intNumber % 10]);
    intNumber /= 10;
  }
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}
