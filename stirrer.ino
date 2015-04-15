// Stirrer is controlled over I2C
// There are two functions, one that writes data to the stirrer
// and other that extracts the settings.





  


// Send 14 bytes of settings to the stirrer over I2C
void setStirrer(void)
{
  // Start I2C stansfer
  Wire1.beginTransmission(SlaveDeviceId);

  // Transfer data to I2C byte-by-byte
  for(i = 2; i < serialbuffer[0]; i++)
  {
    Wire1.write(serialbuffer[i]);
  }

  // End I2C transfer
  I2CendTrans();

  // Update the event register 1 to store the setting of the stirrer
  onoff = ((onoff & 0b11111001) | (((serialbuffer[3] + 1) << 1) & 0b00000110));
  ev1 |=  0b001010000;
  // Store event
  callEvent();
  // Update the event register to say that time NOT changed anymore
  // ev1 &= 0b11001111;

  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;
}


// Send 14 bytes of stirrer settings to PC
void getStirrer(void)
{
  // Return checksum and reset serial transfer
  checkSum();
  Sin = 0;
  serlen = 0;

  // Get all settings from stirrer
  readFromStirrer();

  // Send the rest of the data to the PC
  SerialUSB.write(readI2Cbuf, lengthI2Cbuf);
}


// Funciton that read the stirrer settings from its EEPROM
void readFromStirrer(void)
{
  // Start I2C stansfer
  Wire1.beginTransmission(SlaveDeviceId);

  // Send command 'e' (ASCII 101) to the stirrer
  //   Read all settings from the (e)EPROM
  // Expect full length of the readI2Cbuf worth of data
  Wire1.write(101);

  // End I2C transfer
  I2CendTrans();

  // Wait a moment for the I2C transfer to finish
  delayMicroseconds(100);

  // Request data from strirrer (slave)
  // Start I2C stansfer
  Wire1.beginTransmission(SlaveDeviceId);

  // Wait for data to arrive
  byte availBytes = Wire1.requestFrom(SlaveDeviceId, lengthI2Cbuf);

  // Read data from the stirrer byte-by-byte
  for (i = 0; i < lengthI2Cbuf; i++)
  {
    readI2Cbuf[i] = Wire1.read();
  }

  // End I2C transfer
  I2CendTrans();

  // Display output on the LCD
  if (bitRead(err, 4))
  {
    // Display N/A if there is I2C transfer error
    loadLCDdata(45, 78);  // Add an "N" for (N)/A
    loadLCDdata(46, 47);  // Add an "/" for N(/)A
    loadLCDdata(47, 65);  // Add an "A" for N/(A)
  }
  else
  {
    // If there is no error display stuff according to the operation mode
    switch (readI2Cbuf[1])
    {
      case 0:
        // Display Off if operation is 0
        loadLCDdata(45, 79);  // Add an "O" for (O)ff
        loadLCDdata(46, 102); // Add an "f" for O(f)f
        loadLCDdata(47, 102); // Add an "f" for Of(f)
        break;

      case 1:
        // Display On if operation is 1
        loadLCDdata(45, 79);  // Add an "O" for (O)n 
        loadLCDdata(46, 110); // Add an "n" for O(n) 
        loadLCDdata(47, 32);  // Add an " " for On( )
        break;

      case 2:
        // Display Per if operation is 2
        loadLCDdata(45, 80);  // Add an "P" for (P)er
        loadLCDdata(46, 101); // Add an "e" for P(e)r
        loadLCDdata(47, 114); // Add an "r" for Pe(r)
        break;
    }
  }
}


void I2CendTrans(void)
{
  // End I2C transfer
  byte I2Cerror = Wire1.endTransmission();

  // Check if there is any transfer error
  if (I2Cerror)
  {
    err |= 0b00010000;    // Note the stirrer error
    ev2 |= 0b01000000;    // Say there is an error event
    onoff &= 0b11111001;    // Delete current setting of the stirrer
    displayError();
    // Store event
    callEvent();
  }
    else
  {
    if (bitRead(err, 4))
    {
      err &= 0b11101111;
      displayError();
      // Store event
      callEvent();
    }
  }
}
