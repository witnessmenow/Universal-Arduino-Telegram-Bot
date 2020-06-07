/*
Copyright (c) 2018 Brian Lough. All right reserved.

UniversalTelegramBot - Library to create your own Telegram Bot using
ESP8266 or ESP32 on Arduino IDE.

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

#ifndef UniversalTelegramBot_h
#define UniversalTelegramBot_h

//#define TELEGRAM_DEBUG 1
#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_USE_LONG_LONG 1
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>
#include <core_version.h> 

#define TELEGRAM_HOST "api.telegram.org"
#define TELEGRAM_SSL_PORT 443
#define HANDLE_MESSAGES 1

//unmark following line to enable debug mode
//#define _debug

typedef bool (*MoreDataAvailable)();
typedef byte (*GetNextByte)();
typedef byte* (*GetNextBuffer)();
typedef int (GetNextBufferLen)();

struct telegramMessage {
  String text;
  String chat_id;
  String chat_title;
  String from_id;
  String from_name;
  String date;
  String type;
  float longitude;
  float latitude;
  int update_id;
};

class UniversalTelegramBot {
public:
  UniversalTelegramBot(String token, Client &client);
  String sendGetToTelegram(String command);
  String sendPostToTelegram(String command, JsonObject payload);
  String
  sendMultipartFormDataToTelegram(String command, String binaryProperyName,
                                  String fileName, String contentType,
                                  String chat_id, int fileSize,
                                  MoreDataAvailable moreDataAvailableCallback,
                                  GetNextByte getNextByteCallback, 
								  GetNextBuffer getNextBufferCallback, 
								  GetNextBufferLen getNextBufferLenCallback);

  bool getMe();

  bool sendSimpleMessage(String chat_id, String text, String parse_mode);
  bool sendMessage(String chat_id, String text, String parse_mode = "");
  bool sendMessageWithReplyKeyboard(String chat_id, String text,
                                    String parse_mode, String keyboard,
                                    bool resize = false, bool oneTime = false,
                                    bool selective = false);
  bool sendMessageWithInlineKeyboard(String chat_id, String text,
                                     String parse_mode, String keyboard);

  bool sendChatAction(String chat_id, String text);

  bool sendPostMessage(JsonObject payload);
  String sendPostPhoto(JsonObject payload);
  String sendPhotoByBinary(String chat_id, String contentType, int fileSize,
                           MoreDataAvailable moreDataAvailableCallback,
                           GetNextByte getNextByteCallback, 
						   GetNextBuffer getNextBufferCallback, 
						   GetNextBufferLen getNextBufferLenCallback);
  String sendPhoto(String chat_id, String photo, String caption = "",
                   bool disable_notification = false,
                   int reply_to_message_id = 0, String keyboard = "");

  int getUpdates(long offset);
  bool checkForOkResponse(String response);
  telegramMessage messages[HANDLE_MESSAGES];
  long last_message_received;
  String name;
  String userName;
  int longPoll = 0;
  int waitForResponse = 1500;
  bool _debug = false;

private:
  // JsonObject * parseUpdates(String response);
  String _token;
  Client *client;
  void closeClient();
  const int maxMessageLength = 1500;
  bool processResult(JsonObject result, int messageIndex);
};

#endif
