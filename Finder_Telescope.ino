#include "Wire.h"
#include <UniversalTelegramBot.h>
/////////////////////////////автоподключение к вайфай
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
////////////////////////////
#include <Ticker.h>  //реализация многозадачности
///////////////////////////
#include <NTPClient.h>
#include <WiFiUdp.h>
//////////////////////////////////////////////////
#include <QMC5883LCompass.h>

QMC5883LCompass compass;
/////////////////////////////////////mag
#include <EEPROM.h>
///////////////////////
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//////////////////////////////////////





#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

///////////////////////////////////////

#define EEPROM_SIZE 6  // This is 2-Byte

#define I2C_MPU6050_ADDR 0x69
#define I2C_RTC3231_ADDR 0x68
#define I2C_QMC5883L_ADDR 0x0D
#define I2C_EEPROM 0x52

/*#define WIFI_SSID "guest"
#define WIFI_PASSWORD "#Knowledge-Pool@"*/

//Telegram BOT Token (Get from Botfather)
#define BOTtoken "7922606804:AAGeDPklXF4w5p8UwLzOwMb6jraf9k361r4"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
Ticker Gyro_accel;


WiFiManager wm;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int botRequestDelay = 4000;
unsigned long lastTimeBotRan;


short int Accel_X, Accel_Y, Accel_Z, Gyro_X, Gyro_Y, Gyro_Z;
int Temp_gyro, Temp_mag;
float Temp_gyro_translated, Temp_mag_translated, X, Y;
signed int gradusX = 0, gradusY = 0, gradusZ = 0;
bool key_calibrate = 0, key_tracking = 0;

int mid_Temp[10][500], num_temp[10], Temp_sum[10];

signed short int Calubrate_Gyro_X = 0, Calubrate_Gyro_Y = 0, Calubrate_Gyro_Z = 0;

byte second, minute, hour, Second, Minute, Hour;

float year, day;


float tracing_x, tracing_y, declension = 48.4603, declension_star = 54.7886, Right_ascension_gradus = 35.32777, Right_ascension_gradus_star = 13.4158;
float tracing_x_save = 0, tracing_y_save = 0;
void setup() {

  Serial.begin(9600);
  Wire.begin();
  compass.init();  ///////////////////////////////
  Init_wifi();
  Init_bot();
  init_oled();

  Setting_accelerometer_gyroscope();

  timeClient.begin();  //NTP
  timeClient.update();

  Setting_RTC();

  Write_i2c(I2C_QMC5883L_ADDR, 0x0B, 0b00000001);
  Write_i2c(I2C_QMC5883L_ADDR, 0x09, 0b00000001);


  EEPROM.begin(EEPROM_SIZE);


  compass.init();

  compass.setCalibrationOffsets(668.00, -234.00, 208.00);
  compass.setCalibrationScales(0.55, 1.60, 1.77);


  delay(100);

  /*int data;
//Write_i2c(I2C_EEPROM, 0x00, 54);

  Wire.beginTransmission(I2C_EEPROM);  // transmit to device #8
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(24);
  Wire.endTransmission();

delay(10);

 Wire.beginTransmission(I2C_EEPROM);  // transmit to device #8
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();


  Wire.requestFrom(I2C_EEPROM, 1);  // Request 6 bytes from slave
  // Slave may send less than requested
  while (Wire.available()) data = Wire.read();  // Receive a byte as character*/

  //data=Read_i2c(I2C_EEPROM,0x00);

  Calubrate_Gyro_X = EEPROM.read(0x00);
  Calubrate_Gyro_X |= EEPROM.read(0x01) << 8;
  Calubrate_Gyro_Y = EEPROM.read(0x02);
  Calubrate_Gyro_Y |= EEPROM.read(0x03) << 8;
  Calubrate_Gyro_Z = EEPROM.read(0x04);
  Calubrate_Gyro_Z |= EEPROM.read(0x05) << 8;
  //Serial.println(data);
  Serial.println(Calubrate_Gyro_X);
  Serial.println(Calubrate_Gyro_Y);
  Serial.println(Calubrate_Gyro_Z);


  year = (timeClient.getEpochTime() / 31536000.0);
  day = (year - (int)year) * 365 - (int)((55.0 / 4.0) - 0.5);

  Gyro_accel.attach_ms(20, Gyro_accel_fun);

  Update_Time();
  tracking();
}

int x, y, z, gradus;

void loop() {

  int x;

  // Read compass values
  /* compass.read();
  gradus = compass.getAzimuth();*/
  compass.read();
  Serial.print(" event.magnetic.x = ");
  Serial.println(compass.getX());


  Serial.print(", ");
  Serial.println(compass.getY());


  Serial.print(", ");
  Serial.println(compass.getZ());

  Serial.print(", ");
  Serial.println(pow(pow(compass.getZ(), 2) + pow(compass.getX(), 2) + pow(compass.getY(), 2), 0.5));


  //////////1559


  delay(1000);

  if (key_calibrate) Calibrate_gyro();
  if (key_tracking) tracking();

  Update_Time();
  // oled();

  /* Serial.println();
  Serial.println();
  Serial.println();
  Serial.print("tracing_x ");
  Serial.println(tracing_x - tracing_x_save);

  Serial.print("tracing_y ");
  Serial.println(tracing_y - tracing_y_save);*/

  /*
  Temp_mag = Read_i2c(I2C_QMC5883L_ADDR, 0x07) << 8;
  Serial.print(Temp_mag);
  Temp_mag = Read_i2c(I2C_QMC5883L_ADDR, 0x08);

  Serial.print("Temp_mag:");
  Serial.println((float)Temp_mag / 100.0);*/
  /*
  Serial.print("gradus-");
  Serial.println(gradus);*/

  Gyro_accel_fun();

  Temp_gyro = fun(0x41, 0x42);
  Temp_gyro_translated = (Temp_gyro / 340.0) + 36.53;
  // Serial.print("Temp_gyro:");
  // Serial.println((Temp_gyro / 340.0) + 36.53);


  Serial.print("Gyro_X:");
  Serial.println((float)gradusX / 6550);

  Serial.print("Gyro_Y:");
  Serial.println((float)gradusY / 6550);

  Serial.print("Gyro_Z:");
  Serial.println((float)gradusZ / 6550);

  Serial.print("X:");
  X = atan((float)Accel_X / pow(pow(Accel_Z, 2) + pow(Accel_Y, 2), 0.5)) * 57.296;
  Serial.println(X);

  Serial.print("Y:");
  Y = atan((float)Accel_Y / pow(pow(Accel_Z, 2) + pow(Accel_X, 2), 0.5)) * 57.296;
  Serial.println(Y);


  //float tracing_x, tracing_y, declension = 48.423753, declension_star = 79.23472, Right_ascension_gradus = 35.004661, Right_ascension_gradus_star = 5.377583;


  // tracing_x = declension + (declension_star - 90) * sin((90 + Hour * 15 + Minute * 0.25 + Second * 0.0041667 - Right_ascension_gradus_star * 15 + Right_ascension_gradus - (80 - day) * 0.9856) / 57.29578) ;
  //tracing_y = (declension_star - 90) * cos((90 + Hour * 15 + Minute * 0.25 + Second * 0.0041667 - Right_ascension_gradus_star * 15 + Right_ascension_gradus - (86 - day) * 0.9856) / 57.29578);


  tracing_x = asin(sin(declension / 57.2957795) * sin(declension_star / 57.2957795) + cos(declension / 57.2957795) * cos(declension_star / 57.2957795) * cos((180 + Hour * 15 + Minute * 0.25 + Second * 0.0041667 - Right_ascension_gradus_star * 15 + Right_ascension_gradus - (82 - day) * 0.9856) / 57.2957795)) * 57.2957795;
  tracing_y = acos((sin(declension / 57.2957795) * cos(declension_star / 57.2957795) * cos((180 + Hour * 15 + Minute * 0.25 + Second * 0.0041667 - Right_ascension_gradus_star * 15 + Right_ascension_gradus - (82 - day) * 0.9856) / 57.2957795) - cos(declension / 57.2957795) * sin(declension_star / 57.2957795)) / (sin((90 - tracing_x) / 57.2957795))) * 57.2957795;


  //=ASIN(SIN(0,8464)*SIN(0,956)+COS(0,8464)*COS(0,956)*COS(A1*0,08726646))*57,2957795
  //=ACOS((SIN(0,8464)*COS(0,956)*COS(0,08726646*A1)-COS(0,8464)*SIN(0,956))/(SIN((90-T1)/57,2957795)))*57,2957795
  //sin 𝛼 = sin 𝐿 sin 𝛿 + cos 𝐿 cos 𝛿 cos ℎ,
  /*
  Serial.print("- (80 - day) * 0.9856 =");
  Serial.println(-(80 - day) * 0.9856);
  Serial.print("- Right_ascension_gradus_star * 15 =");
  Serial.println(-Right_ascension_gradus_star * 15);
  Serial.print("Hour =");
  Serial.println(Hour * 15 + Minute * 0.25 + Second * 0.0041667);
  Serial.print("declension_star - 90 =");
  Serial.println(declension_star - 90);
  */
  /* Serial.print("sin(60) ");
  Serial.println(sin(60/57.2957),4);
    Serial.print("asin(0.5) ");
  Serial.println(asin(0.5)*57.2957,4);*/


  /*
  Serial.print("tracing_y = ");
  Serial.print((int)tracing_y);
  Serial.print(" ");
  Serial.print((tracing_y - (int)tracing_y) * 60 / 100);
  Serial.println("'      ");

  Serial.print("tracing_x = ");
  Serial.print((int)tracing_x);
  Serial.print(" ");
  Serial.print((tracing_x - (int)tracing_x) * 60 / 100);
  Serial.println("'      ");
  */

  Communication_Bot();
}
