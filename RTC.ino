

void initRTC(void)
{
  // Start I2C communication on bus Wire1
  Wire1.begin();
  // Start communication with DS1338
  rtc.begin();
  // Initialize the internal RTC
  rtc_clock.init();
  // Start the second interrupt
  //NVIC_EnableIRQ(RTC_IRQn);
  //RTC_EnableIt(RTC, RTC_IER_SECEN);
  // Read the current time from DS1338
  DateTime now = rtc.now();
  // Update time and date of the internal RTC
  rtc_clock.set_time(now.hour(), now.minute(), now.second());
  rtc_clock.set_date(now.day(), now.month(), now.year());
  // Attach an interrupt function for every second tick
  rtc_clock.attachsec(secondInterrupt);
  // Make sure external RTC is not giving errors
  RTCerrorCheck();
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

void RTCerrorCheck(void)
{
  if (! rtc.isrunning())
  {
    // If the RTC is not running because it has seen power down
    err = err | B1;
    // Store event
    //callEvent();
  }
  else
  {
    err = err & B11111110;
  }

  /*if (rtc.oscstopflag())
  {
    // If the RTC has had problems with the oscillator
    // See OSF on page 11 of DS1338 datasheet for details
    err = err | B10;
    // Store event
    //callEvent();
  }
  else
  {
    err = err & B11111101;
  }*/
  // Update the error message on the display 
  displayError();
}
