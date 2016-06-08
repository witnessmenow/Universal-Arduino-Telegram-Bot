/*
  Copyright (c) 2015 Giancarlo Bacchio. All right reserved.

  TelegramBot - Library to create your own Telegram Bot using
  ESP8266 on Arduino IDE.
  Ref. Library at https:github/esp8266/Arduino

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
  */


#ifndef ESP8266TelegramBOT_h
#define ESP8266TelegramBOT_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TelegramBotCore.h>

#define MAX_BUFFER_SIZE 1000

class ESP8266TelegramBOT: public TelegramBotCore
{
  public:
    ESP8266TelegramBOT (String);
    String sendGetToTelegram(String command);

  private:
    WiFiClientSecure client;
    const int maxMessageLength = 1000;

};

#endif
