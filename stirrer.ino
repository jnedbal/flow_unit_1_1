// Stirrer is controlled over I2C
// There are two functions, one that writes data to the stirrer
// and other that extracts the settings.


// I2C address
//    Make sure this address is unique on the I2C bus and identical in the stirrer
#define SlaveDeviceId 9
#define lengthI2Cbuf 14



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
  Wire1.endTransmission();

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

  // Start I2C stansfer
  Wire1.beginTransmission(SlaveDeviceId);

  // Send command 'e' (ASCII 101) to the stirrer
  //   Read all settings from the (e)EPROM
  // Expect full length of the readI2Cbuf worth of data
  Wire1.write(101);

  // End I2C transfer
  Wire1.endTransmission();

  // Wait a moment for the I2C transfer to finish
  delayMicroseconds(100);

  // Request data from strirrer (slave)
  // Start I2C stansfer
  Wire1.beginTransmission(SlaveDeviceId);

  // Wait for data to arrive
  byte availBytes = Wire1.requestFrom(SlaveDeviceId, lengthI2Cbuf);

  // Pass on the data read from the stirrer onto the computer
  byte readI2Cbuf[lengthI2Cbuf];
  for (i = 0; i < lengthI2Cbuf; i++)
  {
    readI2Cbuf[i] = Wire1.read();
  }

  // End I2C transfer
  Wire1.endTransmission();

  // Send the rest of the data to the PC
  SerialUSB.write(readI2Cbuf, lengthI2Cbuf);
}
