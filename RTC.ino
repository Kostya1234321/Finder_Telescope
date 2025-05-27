void Update_Time() {
  second = Read_i2c(I2C_RTC3231_ADDR, 0x00);
  minute = Read_i2c(I2C_RTC3231_ADDR, 0x01);
  hour = Read_i2c(I2C_RTC3231_ADDR, 0x02);

  Second = (second & (0b00001111)) + (10 * ((second & (0b01110000)) >> 4));
  Minute = (minute & (0b00001111)) + (10 * ((minute & (0b01110000)) >> 4));
  Hour = hour;
}

void Setting_RTC()
{
  second = timeClient.getSeconds();
  second = (second % 10) + ((second / 10) << 4);

  minute = timeClient.getMinutes();
  minute = (minute % 10) + ((minute / 10) << 4);

  hour = timeClient.getHours();


  Write_i2c(I2C_RTC3231_ADDR, 0x00, second);
  Write_i2c(I2C_RTC3231_ADDR, 0x01, minute);
  Write_i2c(I2C_RTC3231_ADDR, 0x02, hour);
}
