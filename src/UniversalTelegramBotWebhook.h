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

#ifndef UniversalTelegramBotWebhook_h
#define UniversalTelegramBotWebhook_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>
#include <Server.h>
#include "UniversalTelegramBot.h"

int find_text(String sInput, String sSubStr) {
  int fpos = -1;
  for (int i = 0; i <=sInput.length() - sSubStr.length(); i++) {
    if (sInput.substring(i,sSubStr.length()+i) == sSubStr) {
      fpos = i;
    }
  }
  return fpos;
}

class UniversalTelegramBotWebhook : public UniversalTelegramBot
{
  public:
    UniversalTelegramBotWebhook (String botToken, String secretPOST, Client &client, Server &server);
	int getUpdates() override;
   
  private:
    //String _token;
	String _secretPOST;
    //Client *client;
	Server *server;
	bool isTelegramClientConnected()
};

#endif
