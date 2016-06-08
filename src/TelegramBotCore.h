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


#ifndef TelegramBotCore_h
#define TelegramBotCore_h

#include <Arduino.h>
#include <ArduinoJson.h>

#define HOST "api.telegram.org"
#define SSL_PORT 443
#define HANDLE_MESSAGES 1
#define MAX_BUFFER_SIZE 1000

struct telegramMessage{
  String text;
  String chat_id;
  String sender;
  String date;
  int update_id;
};

class TelegramBotCore
{
  public:
    TelegramBotCore (String);
    virtual String sendGetToTelegram(String command) = 0;
    void begin(void);
    bool getMe();
    void sendMessage(String chat_id, String text, String reply_markup);
    int getUpdates(int offset);
    telegramMessage messages[HANDLE_MESSAGES];
    int last_message_recived;
    String name;
    String userName;
    const char* fingerprint = "37:21:36:77:50:57:F3:C9:28:D0:F7:FA:4C:05:35:7F:60:C1:20:44";  //Telegram.org Certificate

  private:
    String _token;
};

#endif
