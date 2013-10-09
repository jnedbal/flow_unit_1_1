

void initRTC(void)
{
  rtc.begin();  // Start communication with DS1338
  // Check if RTC is running

  if (! rtc.isrunning())
  {
    // If the RTC is not running because it has seen power down
    err = err | B1;
    // following line sets the RTC to the date & time 
    // this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  else
  {
    err = err & B11111110;
  }

  if (rtc.oscstopflag())
  {
    // If the RTC has had problems with the oscillator
    // See OSF on page 11 of DS1338 datasheet for details
    err = err | B10;
  }
  else
  {
    err = err & B11111101;
  }

  if (err)
  {
    // If error has occurred, print it on the display
    lcd.setCursor(17, 0);
    lcd.print("E");
    lcd.print(err);
  }
  else
  {
    lcd.setCursor(17, 0);
    lcd.print("   ");
  }

  // Initialize the internal RTC
  rtc_clock.init();
  // Start the second interrupt
  //NVIC_EnableIRQ(RTC_IRQn);
  //RTC_EnableIt(RTC, RTC_IER_SECEN);
  // Read the current time from DS1338
  DateTime now = rtc.now();
  // Update time and date of internal RTC
  rtc_clock.set_time(now.hour(), now.minute(), now.second());
  rtc_clock.set_date(now.day(), now.month(), now.year());
  // Attach an interrupt function for every second tick
  rtc_clock.attachsec(secondInterrupt);
}

void secondInterrupt(void)
{
  //uint32_t ul_status = rtc_clock.rtc_get_status(RTC);
  digitalWrite(13, ledOn?HIGH:LOW); //Turn the LED on or off, depending on if ledOn is true or false
  ledOn = ledOn?false:true; //Toggle between false and true
  /* Second increment interrupt */
  //if (RTC_SR_SEC)
  //{
    //digitalWrite(13, digitalRead(13)?LOW:HIGH);
  //  RTC_SR_SEC = 0;
    //rtc_clock.rtc_clear_status(RTC, RTC_SCCR_SECCLR);
  //}
}
