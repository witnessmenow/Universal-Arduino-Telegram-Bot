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

#include "UniversalTelegramBotWebhook.h"

UniversalTelegramBotWebhook::UniversalTelegramBotWebhook(String botToken, String secretPOST, Client &client, Server &server) {
  _token = botToken;
  _secretPOST = secretPOST;
  this->client = &client;
  this->server = &server;
}

int UniversalTelegramBotWebhook::getUpdates()  {
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("GET Update Messages");
  #endif
  
  client = server -> available();
  
  if (!client) return 0;
  
  if isTelegramClientConnected() {
	  
  #ifdef DEBUG_U_TelegramBot
  Serial.println("Telegram client connected");
  #endif
  
	  if (client -> available()) {
		client.setTimeout(1000);
			String tgWebhook = client.readString();
    	int bodyStart = find_text(tgWebhook, "\r\n\r\n") + 4; // Dirty hack, but works fast.
    
	#ifdef DEBUG_U_TelegramBot
    Serial.println("BodyStart is "+String(bodyStart));
    #endif
    
    String tgHTTPheader = tgWebhook.substring(0, bodyStart);
    String tgHTTPbody = tgWebhook.substring(bodyStart);
    
	
    if (tgHTTPheader.length() > 0) {
		
	// Here must be code to compare POST and SECRET POST
		
	
	#ifdef DEBUG_U_TelegramBot
     Serial.println("----BEGIN HEADERS-------------------------------");
     Serial.println(tgHTTPheader);
     Serial.println("----END HEADERS---------------------------------");  
	#endif
    }
	
	
    if (tgHTTPbody.length() > 0) {
	#ifdef DEBUG_U_TelegramBot	
     Serial.println("----BEGIN BODY-------------------------------");
     Serial.println(tgHTTPbody);
     Serial.println("----END BODY---------------------------------");  
	#endif
	   // Prepare response for Telegram server. Our politics is keep-alive connection as long as possible
    String s = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n\r\n";
    
    // Send the response to the client
    client.print(s);
	
	#ifdef DEBUG_U_TelegramBot
    Serial.println("Response sent to client: " + s); 
	#endif
    
	return parseTelegramResponse(tgHTTPbody);
    }

	}
	  
	return 0;  
  }
  

  
  
  if (response != "") {
	#ifdef DEBUG_U_TelegramBot  
    if (_debug)  {
      Serial.print("incoming message length");
      Serial.println(response.length());
      Serial.println("Creating DynamicJsonBuffer");
    }
	#endif
	 return parseTelegramResponse(response);
	}
    return 0;
}

bool UniversalTelegramBotWebhook::isTelegramClientConnected() {
	return client -> connected();
}