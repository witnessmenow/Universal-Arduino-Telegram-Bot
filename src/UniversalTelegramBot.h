/*
Copyright (c) 2015 Giancarlo Bacchio. All right reserved.
Copyright (c) Brian Lough
Webhook support by Denis G Dugushkin (c) 2017

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

#ifndef UniversalTelegramBot_h
#define UniversalTelegramBot_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define HOST "api.telegram.org"
#define SSL_PORT 443
//#define TG_HTTP_ANSWER "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n\r\n"
#define TG_HTTP_ANSWER "HTTP/1.1 200 OK\r\n\r\n"

#define HANDLE_MESSAGES 1
#define MAX_MESSAGE_LENGTH 1300
#define CLIENT_TIMEOUT 1500
#define RESPONSE_TIMEOUT 8000

// This will save you some memory (~1.5kb) due to excluding dead debug code
// ESP8266 compiler can't remove dead code because "_debug" variable can be assigned at runtime
// Remove comment below to use debug code

#define DEBUG_U_TelegramBot 

typedef bool (*MoreDataAvailable)();
typedef byte (*GetNextByte)();

struct telegramMessage{
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

class UniversalTelegramBot
{  
  public:
    UniversalTelegramBot (String token, Client &client);
	UniversalTelegramBot (String token, String botSecretURI);
	void setClient(Client &client);
	
    String sendGetToTelegram(String command);
    String sendPostToTelegram(String command, JsonObject& payload);
    String sendMultipartFormDataToTelegram(String command, String binaryProperyName,
        String fileName, String contentType,
        String chat_id, int fileSize,
        MoreDataAvailable moreDataAvailableCallback,
        GetNextByte getNextByteCallback);
		
    bool getMe();
    bool sendSimpleMessage(String chat_id, String text, String parse_mode);
    bool sendMessage(String chat_id, String text, String parse_mode = "");
    bool sendMessageWithReplyKeyboard(String chat_id, String text,
        String parse_mode, String keyboard, bool resize = false,
        bool oneTime = false, bool selective = false);
    bool sendMessageWithInlineKeyboard(String chat_id, String text,
        String parse_mode, String keyboard);

    bool sendChatAction(String chat_id, String text);

    bool sendPostMessage(JsonObject& payload);
    String sendPostPhoto(JsonObject& payload);
    String sendPhotoByBinary(String chat_id, String contentType, int fileSize,
        MoreDataAvailable moreDataAvailableCallback,
        GetNextByte getNextByteCallback);
    String sendPhoto(String chat_id, String photo, String caption = "",
        bool disable_notification = false, int reply_to_message_id = 0, String keyboard = "");

    int getUpdates(long offset);
	int getUpdates(Client &client);
    bool checkForOkResponse(String response);
    telegramMessage messages[HANDLE_MESSAGES];
    long last_message_received;
    String name;
    String userName;
    int longPoll = 30; // Default value changed to 30s. At least it will save some more World's internet traffic.
	
	#ifdef DEBUG_U_TelegramBot
    bool _debug = true;
	#endif

  private:
    //JsonObject * parseUpdates(String response);
    String _token;
	String _sercretURI;
    Client *client;
	int parseTelegramResponse(String response);
	String parseTelegramHTTPResponse(String response);
    bool processResult(JsonObject& result, int messageIndex);
	bool isConnectedTelegramServer();
	int find_text(String sInput, String sSubStr);
    const int maxMessageLength = MAX_MESSAGE_LENGTH;	
};

#endif
