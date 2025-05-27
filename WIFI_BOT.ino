
void Init_wifi() {

  bool res;
  res = wm.autoConnect("AutoConnectAP");  // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else Serial.println("connected...yeey :)");  //if you get here you have connected to the WiFi*/


  //configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  //secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  /*Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());*/
}



void Init_bot() {

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");  // получаем всемирное координированное время (UTC) через NTP
  client.setTrustAnchors(&cert);     // Получаем сертификат api.telegram.org
#endif

  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
}


void Communication_Bot() {

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }
}

void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {

    String chat_id = bot.messages[i].chat_id;

    // Выводим полученное сообщение
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;

    if (text == "/x") {
      bot.sendMessage(chat_id, String((float)X));
    }
    if (text == "/y") {
      bot.sendMessage(chat_id, String((float)Y));
    }
    if (text == "/temp") {
      bot.sendMessage(chat_id, String(Temp_gyro_translated));
    }
    if (text == "/gx") {
      bot.sendMessage(chat_id, String((float)gradusX / 6550));
    }
    if (text == "/gy") {
      bot.sendMessage(chat_id, String((float)gradusY / 6550));
    }
    if (text == "/gz") {
      bot.sendMessage(chat_id, String((float)gradusZ / 6550));
    }
    if (text == "/second") {
      bot.sendMessage(chat_id, String(Second));
    }
    if (text == "/minute") {
      bot.sendMessage(chat_id, String(Minute));
    }
    if (text == "/hour") {
      bot.sendMessage(chat_id, String(Hour));
    }
    if (text == "/gradus") {
      bot.sendMessage(chat_id, String(gradus));
    }
    if (text == "/calibrate") {
      key_calibrate = 1;
      bot.sendMessage(chat_id, "calibrate");
    }
    if (text.startsWith("/starcoordinates ")) {

      String expressio = text.substring(17);
      expressio.trim();
      sscanf(expressio.c_str(), " %f %f ", &declension_star, &Right_ascension_gradus_star);

      key_tracking = 1;

      bot.sendMessage(chat_id, "/starcoordinates");
      bot.sendMessage(chat_id, String(declension_star));
      bot.sendMessage(chat_id, String(Right_ascension_gradus_star));
    }
    if (text.startsWith("/mycoordinates ")) {

      String expressio = text.substring(15);
      expressio.trim();
      sscanf(expressio.c_str(), " %f %f ", &declension, &Right_ascension_gradus);

      key_tracking = 1;

      bot.sendMessage(chat_id, "/mycoordinates");
      bot.sendMessage(chat_id, String(declension));
      bot.sendMessage(chat_id, String(Right_ascension_gradus));
    }

    if (text.startsWith("/tracking")) {

      key_tracking = 1;

      bot.sendMessage(chat_id, "tracking");
      bot.sendMessage(chat_id, String(tracing_x));
      bot.sendMessage(chat_id, String(tracing_y));
    }
    // Moving_star
    if (text.startsWith("/movingstar")) {
      bot.sendMessage(chat_id, "tracking");
      bot.sendMessage(chat_id, String(tracing_x - tracing_x_save));
      bot.sendMessage(chat_id, String(tracing_y - tracing_y_save));
    }

    if (text == "/Null") {
      gradusX = 0;
      gradusY = 0;
      gradusZ = 0;
    }
  }
}

/*
/x-наклон по горизонту
/y-
/temp-температура прибора
/gx-гироскоп
/gy-гироскоп
/gz-гироскоп
/second-
/minute-
/hour-
/gradus-направление компаса
/calibrate-калибровка гироскопам
/starcoordinates-можно написать координати звезды(высота,время)
/mycoordinates-можно написать свои координаты(широта,долгота)
/tracking-просмотр координат звезды
/movingstar-узнаем на сколько звезда переместилась

/x - наклон по горизонту
/y -
/temp - температура прибора
/gx - гироскоп
/gy - гироскоп
/gz - гироскоп
/second -
/minute -
/hour -
/gradus - направление компаса
/calibrate - калибровка гироскопа
/starcoordinates - можно написать координати звезды(высота,время)
/mycoordinates - можно написать свои координаты(широта,долгота)
/tracking - просмотр координат звезды
/movingstar - узнаем на сколько звезда переместилась

*/
