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
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

class TelegramBOT
{
  public:
    TelegramBOT (String, String, String);
  	String message[3][6];  // amount of messages read per time  (update_id, name_id, name, lastname, chat_id, text)
  	void begin(void);
  	void analizeMessages(void);
  	void sendMessage(String chat_id, String text, String reply_markup);
  	void getUpdates(String offset);
	const char* fingerprint = "37:21:36:77:50:57:F3:C9:28:D0:F7:FA:4C:05:35:7F:60:C1:20:44";  //Telegram.org Certificate 

  private:
    String connectToTelegram(String command);
    String _token;
    String _name;
    String _username;
    WiFiClientSecure client;
};

#endif
