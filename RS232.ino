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
  Serial.write(cksum);
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

  if (!Serial.available() > 0)
  {
    return;
  }
  // Get incoming byte and store it in the serial buffer.
  serialbuffer[Sin] = Serial.read();
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
      /* if the command is D (68 in ASCII) for DAC */
      case 68:
        digitalWrite(13, HIGH);      // assert ~SYNC pin low to make DA converter listen
        for(i = 2; i < serlen; i++)  // loop through last 4 bytes of the array
        {
          //SPI.transfer(serialbuffer[i]);// send over SPI byte by byte to the DA converter
        }

        delay(10);
        digitalWrite(13, LOW);     // assert ~SYNC pin high to finish transfer
        checkSum();

        Sin = 0;
        serlen = 0;
      break;

      /* if the command is D (68 in ASCII) for DAC */
      case 84:
        char ctime[8];
        char cdate[11];
        for(i = 2; i < 10; i++)  // loop through first bytes 3 to 9 of the array
        {
          ctime[i - 2] = (char) serialbuffer[i];
        }
//        lcd.setCursor(0, 1);
//        lcd.print(ctime);
        for(i = 10; i < serlen; i++)  // loop through first bytes 3 to 9 of the array
        {
          cdate[i - 10] = (char) serialbuffer[i];
        }
//        lcd.setCursor(0, 2);
//        lcd.print(cdate);
//        delay(1000);
//        Serial.print(ctime);
//        Serial.print(cdate);
//        Serial.write(serialbuffer, 21);
//        Serial.print(__TIME__);
//        Serial.write(sizeof(ctime));
        rtc.adjust(DateTime(cdate, ctime));
        DateTime now = rtc.now();
        // Update time and date of internal RTC
        rtc_clock.set_time(now.hour(), now.minute(), now.second());
        rtc_clock.set_date(now.day(), now.month(), now.year());
        checkSum();
        Sin = 0;
        serlen = 0;
      break;
    }
  }
}
