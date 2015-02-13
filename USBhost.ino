// USB Host chip requires 12 MHz, it will be created by the Arduino


void init12MHz(void)
{
  // Code from:
  // http://forum.arduino.cc/index.php?topic=218033.0
  // put your setup code here, to run once:
  REG_PIOA_WPMR = 0x50494F00;  // enable writes
  REG_PIOA_PDR = 0x02;         // enable peripheral control
  REG_PIOA_ABSR = 0x02;         // A-B select?
  REG_PIOA_WPMR = 0x50494F01;  // disable writes

  REG_PMC_WPMR = 0x504D4300;   // enable writes
  REG_PMC_SCER = 0x100;
  PMC->PMC_PCK[0] = 0x1;       // master clock = 12Mhz (crystal freq)
  REG_PMC_WPMR = 0x504D4301;   // disable writes
}

