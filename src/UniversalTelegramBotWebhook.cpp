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

int find_text(String sInput, String sSubStr) {
  int fpos = -1;
  for (int i = 0; i <=sInput.length() - sSubStr.length(); i++) {
    if (sInput.substring(i,sSubStr.length()+i) == sSubStr) {
      fpos = i;
    }
  }
  return fpos;
}


UniversalTelegramBotWebhook::UniversalTelegramBotWebhook(String token, Client &client) : UniversalTelegramBot(token, client)
{

}

/*
UniversalTelegramBotWebhook::UniversalTelegramBotWebhook(String botToken, String secretPOST) {
  _token = botToken;
  _secretPOST = secretPOST;
}
*/

int UniversalTelegramBotWebhook::getUpdates()  {
/* 
 
 #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("GET Update Messages");
  #endif
  
  client = this->hserver -> available();
  
  if (!client) return 0;
  
  if (isTelegramClientConnected()) {
	  
  #ifdef DEBUG_U_TelegramBot
  Serial.println("Telegram client connected");
  #endif
  
	  if (client -> available()) {
		client -> setTimeout(1000);
		String tgWebhook = client -> readString();
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
    client -> print(s);
	
	#ifdef DEBUG_U_TelegramBot
    Serial.println("Response sent to client: " + s); 
	#endif
    
	return parseTelegramResponse(tgHTTPbody);
    }

	}
  }
*/  
	return 0;  
}

bool UniversalTelegramBotWebhook::isTelegramClientConnected() {
	return 0; //client -> connected();
}