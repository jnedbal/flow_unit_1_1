// Section NVRAM deals with the non-volatile
// ram 23LCV1024 backed-up with a battery.



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
  byte writeInstr = 0x02;
  SPI.transfer(4, writeInstr, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0x01, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0xA6, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0xBA, SPI_CONTINUE);
  delayMicroseconds(2);
  SPI.transfer(4, 0x34);
  byte readInstr = 0x03;
    SPI.transfer(4, readInstr, SPI_CONTINUE);
  SPI.transfer(4, (byte) ((addr >> 16) & 0xFF), SPI_CONTINUE);
  SPI.transfer(4, (byte) ((addr >> 8) & 0xFF), SPI_CONTINUE);
  SPI.transfer(4, (byte) (addr & 0xFF), SPI_CONTINUE);
  byte answer = SPI.transfer(4, 0x00);
  lcd.setCursor(2, 2);
  lcd.print((byte) answer, HEX);
  lcd.setCursor(5, 2);
  lcd.print((byte) 0x34, HEX);
  //lcd.setCursor(2, 3);
  //lcd.print((byte) ((addr >> 8) & 0xFF), HEX);
  delay(10);
  
  
//  for (addr=0; addr++; addr<256)
//  {
//    SPI.transfer(4, writeInstr, SPI_CONTINUE);
//    SPI.transfer(4, (byte) addr >> 16, SPI_CONTINUE);
//    SPI.transfer(4, (byte) addr >> 8, SPI_CONTINUE);
//    SPI.transfer(4, (byte) addr, SPI_CONTINUE);
//    SPI.transfer(4, (byte) addr);
//  }
//  for (addr=0; addr++; addr<256)
//  {
//    SPI.transfer(4, readInstr, SPI_CONTINUE);
//    SPI.transfer(4, (byte) addr >> 16, SPI_CONTINUE);
//    SPI.transfer(4, (byte) addr >> 8, SPI_CONTINUE);
//    answer = SPI.transfer(4, (byte) addr);
//    lcd.setCursor(2, 0);
//    lcd.print(answer, HEX);
//    delay(100);
//  }  
}
