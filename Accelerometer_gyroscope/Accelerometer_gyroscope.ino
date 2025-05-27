void Setting_accelerometer_gyroscope() {
  // delay(2000);
  Write_i2c(I2C_MPU6050_ADDR, 0x6B, 0);
  Write_i2c(I2C_MPU6050_ADDR, 0x6C, 0);
  Write_i2c(I2C_MPU6050_ADDR, 0x19, 0);
  Write_i2c(I2C_MPU6050_ADDR, 0x1A, 5);
  /* Write_i2c(0X1B, 224);
  Write_i2c(0x1C, 224);
  delay(1000);*/
  Write_i2c(I2C_MPU6050_ADDR, 0X1B, 0);
  Write_i2c(I2C_MPU6050_ADDR, 0x1C, 0);
}

int mid_gyro_X, mid_gyro_Y, mid_gyro_Z, arr_gyro_X[100], arr_gyro_Y[100], arr_gyro_Z[100];

void Gyro_accel_fun() {
  Accel_X = fun(0x3b, 0x3c);
  // Serial.print("X:");
  // Serial.println(X);


  Accel_Y = fun(0x3d, 0x3e);
  //Serial.print("Y:");
  //Serial.println(Y);


  Accel_Z = fun(0x3f, 0x40);
  // Serial.print("Z:");
  //Serial.println(Z);


  Gyro_X = fun(0x43, 0x44);
  //Serial.print("X:");
  //Serial.println(Gyro_X);

  Gyro_Y = fun(0x45, 0x46);
  //Serial.print("Y:");
  //Serial.println(Gyro_Y);

  Gyro_Z = fun(0x47, 0x48);
  // Serial.print("Z:");
  // Serial.println(Gyro_Z);

  mid_gyro_X = 0;
  mid_gyro_Y = 0;
  mid_gyro_Z = 0;
  //Serial.println(mid_gyro_X);
  ////////////////////////////////////////////////////
  /*
for(int i=98;i>=0;i--)arr_gyro_X[i+1]=arr_gyro_X[i];

arr_gyro_X[0]=Gyro_X;

for(int i=0;i<100;i++)mid_gyro_X += arr_gyro_X[i];

mid_gyro_X = mid_gyro_X / 100;*/
  /////////////////////////////////////////////////////


  for (int i = 28; i >= 0; i--) arr_gyro_X[i + 1] = arr_gyro_X[i];

  arr_gyro_X[0] = Gyro_X;

  //Serial.println(mid_gyro_X);


  for (int i = 28; i >= 0; i--) arr_gyro_Y[i + 1] = arr_gyro_Y[i];

  arr_gyro_Y[0] = Gyro_Y;

  ////////////////////////////////////////////////////
  for (int i = 28; i >= 0; i--) arr_gyro_Z[i + 1] = arr_gyro_Z[i];

  arr_gyro_Z[0] = Gyro_Z;


  ///////////////////////////////////////////////////



  /*
for(int j=0;j<10;j++){
  if ( ( ((Temp_gyro / 340.0) + 36.53) > 21.0+j*0.5) && ( ((Temp_gyro / 340.0) + 36.53) < 21.5+j*0.5)) {
    mid_Temp[j][num_temp[j]] = Gyro_X ;

    //Serial.println(Gyro_X);
    if (num_temp[j] < 500) num_temp[j]++;
    
    if (num_temp[j] == 500) {
      for (int i = 0; i < 500; i++) {
        Temp_sum[j] = Temp_sum[j] + mid_Temp[j][i];
      }
      num_temp[j]++;
    }
  }
}
*/


  if (((Gyro_X - Calubrate_Gyro_X) < 70) && ((Gyro_X - Calubrate_Gyro_X) > -70)) gradusX = gradusX;
  else gradusX = gradusX + Gyro_X - Calubrate_Gyro_X;

  if (((Gyro_Y - Calubrate_Gyro_Y) < 70) && ((Gyro_Y - Calubrate_Gyro_Y) > -70)) gradusY = gradusY;
  else gradusY = gradusY + Gyro_Y - Calubrate_Gyro_Y;

  if (((Gyro_Z - Calubrate_Gyro_Z) < 70) && ((Gyro_Z - Calubrate_Gyro_Z) > -70)) gradusZ = gradusZ;
  else gradusZ = gradusZ + Gyro_Z - Calubrate_Gyro_Z;
}



void Calibrate_gyro() {

  key_calibrate = 0;

  for (int i = 0; i < 30; i++) {
    mid_gyro_X += arr_gyro_X[i];
    //Serial.println(mid_gyro_X);
  }

  mid_gyro_X = mid_gyro_X / 30;


  for (int i = 0; i < 30; i++) mid_gyro_Y += arr_gyro_Y[i];

  mid_gyro_Y = mid_gyro_Y / 30;


  for (int i = 0; i < 30; i++) mid_gyro_Z += arr_gyro_Z[i];

  mid_gyro_Z = mid_gyro_Z / 30;


  Serial.println("hjdfeeeegedjhfgehgfehjfgiejfhej");
  Serial.println(Gyro_X);
  Serial.println(Gyro_Y);
  Serial.println(Gyro_Z);
  EEPROM.write(0x00, (signed short int)mid_gyro_X);
  EEPROM.write(0x01, (signed short int)mid_gyro_X >> 8);
  EEPROM.write(0x02, (signed short int)mid_gyro_Y);
  EEPROM.write(0x03, (signed short int)mid_gyro_Y >> 8);
  EEPROM.write(0x04, mid_gyro_Z);
  EEPROM.write(0x05, mid_gyro_Z >> 8);

  EEPROM.commit();

  Calubrate_Gyro_X = mid_gyro_X;
  Calubrate_Gyro_Y = mid_gyro_Y;
  Calubrate_Gyro_Z = mid_gyro_Z;



  Serial.println("mid_gyro_X ");
  Serial.println(mid_gyro_X);
  Serial.println(Gyro_X);
  Serial.println(mid_gyro_Y);
  Serial.println(Gyro_Y);
  Serial.println(mid_gyro_Z);
  Serial.println(Gyro_Z);
  Serial.println();
}



void tracking() {
  key_tracking = 0;

  float year = (timeClient.getEpochTime() / 31536000.0);
  float day = (year - (int)year) * 365 - (int)((55.0 / 4.0) - 0.5);

  Serial.println(year, 4);
  Serial.println(day, 4);

  tracing_x = asin(sin(declension/57.2957795)*sin(declension_star/57.2957795)+cos(declension/57.2957795)*cos(declension_star/57.2957795)*cos((180+Hour * 15 + Minute * 0.25 + Second * 0.0041667 - Right_ascension_gradus_star * 15 + Right_ascension_gradus - (82 - day) * 0.9856) / 57.2957795))*57.2957795;
  tracing_y = acos((sin(declension/57.2957795)*cos(declension_star/57.2957795)*cos((180+Hour * 15 + Minute * 0.25 + Second * 0.0041667 - Right_ascension_gradus_star * 15 + Right_ascension_gradus - (82 - day) * 0.9856) / 57.2957795)- cos(declension/57.2957795)*sin(declension_star/57.2957795))/(sin((90-tracing_x)/57.2957795)))*57.2957795;
 

  Serial.print("tracing_x ");
  Serial.println(tracing_x_save - tracing_x);

  Serial.print("tracing_y ");
  Serial.println(tracing_y_save - tracing_y);

  tracing_x_save = tracing_x;
  tracing_y_save = tracing_y;
}
