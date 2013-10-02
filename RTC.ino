

void initRTC(void)
{
  rtc.begin();  // Start communication with DS1338
  // Check if RTC is running

  if (! rtc.isrunning())
  {
    // If the RTC is not running because it has seen power down
    err = err + B1;
    // following line sets the RTC to the date & time 
    // this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

  if (rtc.oscstopflag())
  {
    // If the RTC has had problems with the oscillator
    // See OSF on page 11 of DS1338 datasheet for details
    err = err + B10;
  }

  if (err)
  {
    // If error has occurred, print it on the display
    lcd.setCursor(17, 0);
    lcd.print("E");
    lcd.print(err);
  }

  // Initialize the internal RTC
  rtc_clock.init();
  // Read the current time from DS1338
  DateTime now = rtc.now();
  // Update time and date of internal RTC
  rtc_clock.set_time(now.hour(), now.minute(), now.second());
  rtc_clock.set_date(now.day(), now.month(), now.year());
}
