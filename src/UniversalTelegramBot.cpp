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

#include "UniversalTelegramBot.h"

// I don't found this trivial function in standart Arduino library.
// It uses to parse HTTP/1 header FAST (but not complex).
inline int UniversalTelegramBot::find_text(String sInput, String sSubStr) {
  int fpos = -1;
  for (int i = 0; i <=sInput.length() - sSubStr.length(); i++) {
    if (sInput.substring(i,sSubStr.length()+i) == sSubStr) {
      fpos = i;
    }
  }
  return fpos;
}

UniversalTelegramBot::UniversalTelegramBot(String token, Client &client)	{
  _token = token;
  this->client = &client;
}

UniversalTelegramBot::UniversalTelegramBot(String token, String botSecretURI) {
  _token = token;
  _sercretURI = botSecretURI;
  
  if (_sercretURI == "") _sercretURI = _token;
}

// This function usable in cases when you want to change Telegram bot behavior from Webhook to LongPoll at Runtime.
void UniversalTelegramBot::setClient(Client &client)	{
  this->client = &client;
}

bool UniversalTelegramBot::isConnectedTelegramServer() {
	
	#ifdef DEBUG_U_TelegramBot	
	Serial.println("Entering UniversalTelegramBot::isConnectedTelegramServer()");
	#endif
	
	int isConnected = client -> connected();
	
	if (!isConnected) {
		
	#ifdef DEBUG_U_TelegramBot	
	Serial.println("Client is not connected");
	#endif
	
	isConnected = client->connect(HOST, SSL_PORT);	
	
	#ifdef DEBUG_U_TelegramBot	
	Serial.println("Connection attemp...");
	#endif
	}
	
	#ifdef DEBUG_U_TelegramBot	
	Serial.println("Exiting UniversalTelegramBot::isConnectedTelegramServer() with result = "+String(isConnected));
	#endif	
	return isConnected;
} 

String UniversalTelegramBot::sendGetToTelegram(String command) {
	String mess = "";
	long now;
	bool avail;

	// Connect with api.telegram.org
	if (!isConnectedTelegramServer()) return mess;
		
	#ifdef DEBUG_U_TelegramBot
    if (_debug) Serial.println(F("Enter UniversalTelegramBot::sendGetToTelegram..."));
	#endif
		

		int ch_count=0;
		client -> println("GET /"+command);
		now=millis();
		avail=false;
		while (millis() - now < longPoll * 1000 + CLIENT_TIMEOUT) {
			while (client->available()) {
				mess = client -> readString();
			}
			if (mess.length() > 0) {
				
		#ifdef DEBUG_U_TelegramBot		
        if (_debug) {
		Serial.println("------------RESPONSE------------");
		Serial.println(mess);
		Serial.println("--------------------------------");
        }
		#endif
		return mess;
			}
		}
	
	#ifdef DEBUG_U_TelegramBot
    if (_debug) {
		Serial.println(F("Exit UniversalTelegramBot::sendGetToTelegram..."));
	}
	#endif

	return mess;
}

String UniversalTelegramBot::sendPostToTelegram(String command, JsonObject& payload){

  String body = "";
  String header = "";

	// Connect with api.telegram.org
	
	header  = "POST /" + command + " HTTP/1.1\r\n"; // POST URI
	header += "Host: " + String(HOST) + "\r\n"; 	 // HOST
	//header += "User-Agent: ESP8266 Universal Telegram Bot\r\n";
	header += "Content-Type: application/json\r\n"; // Content type
	header += "Content-Length: " + String(payload.measureLength()) + "\r\n"; // Content-Length
	header += "Connection: keep-alive";
	//header += "\r\nAccept-Encoding: gzip, deflate";
	header += "\r\n\r\n"; // End of headers

	payload.printTo(body);
	#ifdef DEBUG_U_TelegramBot		
     if (_debug) {
		Serial.println("------------POST------------");
		Serial.println(header);
		Serial.println("--------------------------------");
		Serial.println(body);
		Serial.println("------------END-----------------");
        }
	#endif	
	if (!isConnectedTelegramServer()) return "";
	
	client -> print(header);
	client -> print(body);
	 
	
	String response = "";
	long waitTime = millis() + RESPONSE_TIMEOUT;
	Serial.println("Enter wait response");
	while (millis() < waitTime) {
	delay(10);
		while (client -> available()) {
		String response = this->client -> readString();
		if (response.length() > 0) return parseTelegramHTTPResponse(response);
		delay(100);		
	    }
	}
			
	return "";
}

String UniversalTelegramBot::sendMultipartFormDataToTelegram(String command, String binaryProperyName,
    String fileName, String contentType,
    String chat_id, int fileSize,
    MoreDataAvailable moreDataAvailableCallback,
    GetNextByte getNextByteCallback) {

  String body = "";
  String headers = "";
	long now;
	bool responseReceived;
  String boundry = F("------------------------b8f610217e83e29b");
	// Connect with api.telegram.org
  if (isConnectedTelegramServer()) {

    String start_request = "";
    String end_request = "";

    start_request = start_request + "--" + boundry + "\r\n";
    start_request = start_request + "content-disposition: form-data; name=\"chat_id\"" + "\r\n";
    start_request = start_request + "\r\n";
    start_request = start_request + chat_id + "\r\n";

    start_request = start_request + "--" + boundry + "\r\n";
    start_request = start_request + "content-disposition: form-data; name=\"" + binaryProperyName + "\"; filename=\"" + fileName + "\"" + "\r\n";
    start_request = start_request + "Content-Type: " + contentType + "\r\n";
    start_request = start_request + "\r\n";


    end_request = end_request + "\r\n";
    end_request = end_request + "--" + boundry + "--" + "\r\n";

    client->print("POST /bot"+_token+"/" + command); client->println(F(" HTTP/1.1"));
    // Host header
    client->print(F("Host: ")); client->println(HOST);
    client->println(F("User-Agent: arduino/1.0"));
    client->println(F("Accept: */*"));

    int contentLength = fileSize + start_request.length() + end_request.length();
	
	#ifdef DEBUG_U_TelegramBot
    if (_debug) Serial.println("Content-Length: " + String(contentLength));
	#endif
	
    client->print("Content-Length: "); client->println(String(contentLength));
    client->println("Content-Type: multipart/form-data; boundary=" + boundry);
    client->println("");

    client->print(start_request);

	#ifdef DEBUG_U_TelegramBot
    if (_debug) Serial.print(start_request);
	#endif

    byte buffer[512];
    int count = 0;
    char ch;
    while (moreDataAvailableCallback()) {
      buffer[count] = getNextByteCallback();
      //client->write(ch);
      //Serial.write(ch);
      count++;
      if(count == 512){
        //yield();
		
	  #ifdef DEBUG_U_TelegramBot	
        if (_debug) {
          Serial.println(F("Sending full buffer"));
        }
	  #endif
	  
        client->write((const uint8_t *)buffer, 512);
        count = 0;
      }
    }

    if(count > 0) {
	  #ifdef DEBUG_U_TelegramBot
      if (_debug) {
        Serial.println(F("Sending remaining buffer"));
      }
	  #endif
      client->write((const uint8_t *)buffer, count);
    }

    client->print(end_request);
	
	#ifdef DEBUG_U_TelegramBot
    if (_debug) Serial.print(end_request);
	#endif

    count = 0;
    int ch_count=0;
    char c;
    now=millis();
    bool finishedHeaders = false;
    bool currentLineIsBlank = true;
    while (millis() - now < CLIENT_TIMEOUT) {
      while (client->available()) {
        char c = client->read();
        responseReceived=true;


        if(!finishedHeaders){
          if (currentLineIsBlank && c == '\n') {
            finishedHeaders = true;
          }
          else {
            headers = headers + c;
          }
        } else {
          if (ch_count < maxMessageLength) {
            body=body+c;
            ch_count++;
          }
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }

      }

      if (responseReceived) {
		  
		#ifdef DEBUG_U_TelegramBot  
        if (_debug) {
          Serial.println();
          Serial.println(body);
          Serial.println();
        }
		#endif
		
        break;
      }
    } 	
  }

  return body;
}

bool UniversalTelegramBot::getMe() {
  String command = "bot"+_token+"/getMe";
  String response = sendGetToTelegram(command); //receive reply from telegram.org
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);

  if(root.success()) {
    if (root.containsKey("result")) {
      String _name = root["result"]["first_name"];
      String _username = root["result"]["username"];
      name = _name;
      userName = _username;
      return true;
    }
  }

  return false;
}

/***************************************************************
* GetUpdates - function to receive messages from telegram *
* (Argument to pass: the last+1 message to read)             *
* Returns the number of new messages           *
***************************************************************/
int UniversalTelegramBot::getUpdates(long offset)  {
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("GET Update Messages >> Long poll mode");
  #endif
  
  String command = "bot"+_token+"/getUpdates?offset="+String(offset)+"&limit="+String(HANDLE_MESSAGES);
  if(longPoll > 0) command = command + "&timeout=" + String(longPoll);
  
  String response = sendGetToTelegram(command); //receive reply from telegram.org

  if (response != "") {
	#ifdef DEBUG_U_TelegramBot  
    if (_debug)  {
      Serial.print("Incoming message, length = ");
      Serial.println(response.length());
      Serial.println("Creating DynamicJsonBuffer");
    }
	#endif
	 return parseTelegramResponse(response);
	}
	return 0;
}
	
int UniversalTelegramBot::getUpdates(Client &client)  {
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("GET Update Messages >> WebHook mode");
  #endif
  
  this->client = &client;
  if (!isConnectedTelegramServer()) return 0;
  this->client -> setTimeout(500);
     
  //if (_debug) Serial.println("this->client -> available(); = "+String(this->client -> available()));
 
  String tgWebhook = "";
  long waitTime = millis() + CLIENT_TIMEOUT * 3;
 
	while (millis() < waitTime) {
	delay(100);
	
    while(this->client -> available()) {
	delay(100);	
	#ifdef DEBUG_U_TelegramBot
	if (_debug) Serial.println("this->client -> available(); = True");
	#endif
	    
    tgWebhook = this->client -> readString();
	
    }
	
	if (tgWebhook.length() > 0) {
		this->client -> print(TG_HTTP_ANSWER);
		break; 	
	}
  }
  
  this->client -> flush();	  
  this->client -> stop();
  
  if (tgWebhook.length() > 0)
	if (find_text(tgWebhook,"POST /" + _sercretURI + " HTTP/1.1") != -1) {
	    #ifdef DEBUG_U_TelegramBot
		if (_debug) Serial.println("Exit GET Update Messages >> WebHook mode >> \n"+tgWebhook+"\n");
		#endif
	  return parseTelegramResponse("{\"ok\":true,\"result\":[" + parseTelegramHTTPResponse(tgWebhook) + "]}");
  }
  
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("Exit GET Update Messages >> WebHook mode, = 0");
  #endif
  
   return 0;
}

String UniversalTelegramBot::parseTelegramHTTPResponse(String response) {
	
	
    #ifdef DEBUG_U_TelegramBot
	Serial.println("----BEGIN RESPONSE-------------------------------");
	Serial.println(response);
	Serial.println("----END RESPONSE---------------------------------");  
	#endif
    
    int bodyStart = find_text(response, "\r\n\r\n") + 4; // Dirty hack, but works fast. There could be a good HTTP header parser if we unsure that Telegram server always answers good HTTP headers.
	// HTTP header
    String tgHTTPheader = response.substring(0, bodyStart);
    if (tgHTTPheader.length() == 0) return ""; // something wrong...
	
	#ifdef DEBUG_U_TelegramBot
	 Serial.println("----BEGIN HEADERS-------------------------------");
     Serial.println(tgHTTPheader);
     Serial.println("----END HEADERS---------------------------------");  
   	#endif
	
	// HTTP body
	String tgHTTPbody = response.substring(bodyStart);
    		
	#ifdef DEBUG_U_TelegramBot	
     Serial.println("----BEGIN BODY-------------------------------");
     Serial.println(tgHTTPbody);
     Serial.println("----END BODY---------------------------------");  
	#endif 
	
	return tgHTTPbody;	
}



int UniversalTelegramBot::parseTelegramResponse(String response)  {
   // Parse response into Json object
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);

    if (root.success()) {
      // root.printTo(Serial);
	  #ifdef DEBUG_U_TelegramBot
      if (_debug) Serial.println();
	  #endif
	  
      if (root.containsKey("result")) {
        int resultArrayLength = root["result"].size();
        if (resultArrayLength > 0) {
          int newMessageIndex = 0;
          //Step through all results
          for (int i=0; i < resultArrayLength; i++) {
            JsonObject& result = root["result"][i];
            if (processResult(result, newMessageIndex)) {
              newMessageIndex++;
            }
          }
          return newMessageIndex;
        } 
		#ifdef DEBUG_U_TelegramBot
		  else {		  
          if (_debug) Serial.println(F("no new messages"));
		  
          }
		#endif
      } 
	  #ifdef DEBUG_U_TelegramBot  
	    else {
		if (_debug) Serial.println(F("Response contained no 'result'"));
		
        }
	  #endif
    } 
	#ifdef DEBUG_U_TelegramBot
	else {
      // Buffer may not be big enough, increase buffer or reduce max number of messages
	  if (_debug) Serial.println(F("Failed to parse update, the message could be too big for the buffer"));
	  }
	#endif
	return 0;
}

bool UniversalTelegramBot::processResult(JsonObject& result, int messageIndex) {
  int update_id = result["update_id"];
  // Check have we already dealt with this message (this shouldn't happen!)
  if (last_message_received != update_id) {
    last_message_received = update_id;
    messages[messageIndex].update_id = update_id;

    messages[messageIndex].text = F("");
    messages[messageIndex].from_id = F("");
    messages[messageIndex].from_name = F("");
    messages[messageIndex].longitude = 0;
    messages[messageIndex].latitude = 0;

    if (result.containsKey("message")) {
      JsonObject& message = result["message"];
      messages[messageIndex].type = F("message");
      messages[messageIndex].from_id = message["from"]["id"].as<String>();
      messages[messageIndex].from_name = message["from"]["first_name"].as<String>();

      messages[messageIndex].date = message["date"].as<String>();
      messages[messageIndex].chat_id = message["chat"]["id"].as<String>();
      messages[messageIndex].chat_title = message["chat"]["title"].as<String>();

      if (message.containsKey("text")) {
        messages[messageIndex].text = message["text"].as<String>();

      } else if (message.containsKey("location")) {
        messages[messageIndex].longitude = message["location"]["longitude"].as<float>();
        messages[messageIndex].latitude = message["location"]["latitude"].as<float>();

      }
    }
    else if (result.containsKey("channel_post")) {
      JsonObject& message = result["channel_post"];
      messages[messageIndex].type = F("channel_post");

      messages[messageIndex].text = message["text"].as<String>();
      messages[messageIndex].date = message["date"].as<String>();
      messages[messageIndex].chat_id = message["chat"]["id"].as<String>();
      messages[messageIndex].chat_title = message["chat"]["title"].as<String>();

    } else if (result.containsKey("callback_query")) {
      JsonObject& message = result["callback_query"];
      messages[messageIndex].type = F("callback_query");
      messages[messageIndex].from_id = message["from"]["id"].as<String>();
      messages[messageIndex].from_name = message["from"]["first_name"].as<String>();

      messages[messageIndex].text = message["data"].as<String>();
      messages[messageIndex].date = message["date"].as<String>();
      messages[messageIndex].chat_id = F("");
      messages[messageIndex].chat_title = F("");
    } else if (result.containsKey("edited_message")) {
      JsonObject& message = result["edited_message"];
      messages[messageIndex].type = F("edited_message");
      messages[messageIndex].from_id = message["from"]["id"].as<String>();
      messages[messageIndex].from_name = message["from"]["first_name"].as<String>();

      messages[messageIndex].date = message["date"].as<String>();
      messages[messageIndex].chat_id = message["chat"]["id"].as<String>();
      messages[messageIndex].chat_title = message["chat"]["title"].as<String>();

      if (message.containsKey("text")) {
        messages[messageIndex].text = message["text"].as<String>();

      } else if (message.containsKey("location")) {
        messages[messageIndex].longitude = message["location"]["longitude"].as<float>();
        messages[messageIndex].latitude = message["location"]["latitude"].as<float>();

      }
    }

    return true;
  }
  return false;
}

/***********************************************************************
* SendMessage - function to send message to telegram                  *
* (Arguments to pass: chat_id, text to transmit and markup(optional)) *
***********************************************************************/
bool UniversalTelegramBot::sendSimpleMessage(String chat_id, String text, String parse_mode)  {

  bool sent = false;
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("SEND Simple Message");
  #endif
  
  long sttime = millis();

  if (text!="") {
    while (millis() < sttime + RESPONSE_TIMEOUT) {    // loop for a while to send the message
      String command="bot"+_token+"/sendMessage?chat_id="+chat_id+"&text="+text+"&parse_mode="+parse_mode;
      String response = sendGetToTelegram(command);
	  
      sent = checkForOkResponse(response);
      if (sent) {
        break;
      }
    }
  }

  return sent;
}

bool UniversalTelegramBot::sendMessage(String chat_id, String text, String parse_mode)  {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  payload["chat_id"] = chat_id;
  payload["text"] = text;

  if (parse_mode != "") {
    payload["parse_mode"] = parse_mode;
  }

  return sendPostMessage(payload);
}

bool UniversalTelegramBot::sendMessageWithReplyKeyboard(String chat_id, String text, String parse_mode, String keyboard, bool resize, bool oneTime, bool selective)  {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  payload["chat_id"] = chat_id;
  payload["text"] = text;

  if (parse_mode != "") {
    payload["parse_mode"] = parse_mode;
  }

  JsonObject& replyMarkup = payload.createNestedObject("reply_markup");

  // Reply keyboard is an array of arrays.
  // Outer array represents rows
  // Inner arrays represents columns
  // This example "ledon" and "ledoff" are two buttons on the top row
  // and "status is a single button on the next row"
  DynamicJsonBuffer keyboardBuffer;
  replyMarkup["keyboard"] = keyboardBuffer.parseArray(keyboard);

  //Telegram defaults these values to false, so to decrease the size of the payload we will only send them if needed
  if (resize) {
    replyMarkup["resize_keyboard"] = resize;
  }

  if (oneTime) {
    replyMarkup["one_time_keyboard"] = oneTime;
  }

  if (selective) {
    replyMarkup["selective"] = selective;
  }

  return sendPostMessage(payload);
}

bool UniversalTelegramBot::sendMessageWithInlineKeyboard(String chat_id, String text, String parse_mode, String keyboard)  {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  payload["chat_id"] = chat_id;
  payload["text"] = text;

  if (parse_mode != "") {
    payload["parse_mode"] = parse_mode;
  }

  JsonObject& replyMarkup = payload.createNestedObject("reply_markup");

  DynamicJsonBuffer keyboardBuffer;
  replyMarkup["inline_keyboard"] = keyboardBuffer.parseArray(keyboard);

  return sendPostMessage(payload);
}

/***********************************************************************
* SendPostMessage - function to send message to telegram                  *
* (Arguments to pass: chat_id, text to transmit and markup(optional)) *
***********************************************************************/
bool UniversalTelegramBot::sendPostMessage(JsonObject& payload)  {

  bool sent = false;
  
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println(F("SEND Post Message"));
  #endif
  


  if (payload.containsKey("text")) {
          String command = "bot"+_token+"/sendMessage";
      String response = sendPostToTelegram(command, payload); 
      return checkForOkResponse(response);
 }

  return sent;
}

String UniversalTelegramBot::sendPostPhoto(JsonObject& payload)  {

  bool sent=false;
  String response = "";
  
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println(F("SEND Post Photo"));
  #endif
  
  long sttime=millis() + RESPONSE_TIMEOUT;

  if (payload.containsKey("photo")) {
    while (millis() < sttime ) { // loop for a while to send the message
      String command = "bot"+_token+"/sendPhoto";
      response = sendPostToTelegram(command, payload);
	  #ifdef DEBUG_U_TelegramBot
      if (_debug) Serial.println(response);
	  #endif
	  
      sent = checkForOkResponse(response);
      if (sent) {
        break;
      }
    }
  }

  return response;
}

String UniversalTelegramBot::sendPhotoByBinary(String chat_id, String contentType, int fileSize,
    MoreDataAvailable moreDataAvailableCallback,
    GetNextByte getNextByteCallback) {
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println("SEND Photo");
  #endif

  String response = sendMultipartFormDataToTelegram("sendPhoto", "photo", "img.jpg",
    contentType, chat_id, fileSize,
    moreDataAvailableCallback, getNextByteCallback);
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println(response);
  #endif

  return response;
}

String UniversalTelegramBot::sendPhoto(String chat_id, String photo, String caption, bool disable_notification, int reply_to_message_id, String keyboard) {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  payload["chat_id"] = chat_id;
  payload["photo"] = photo;

  if (caption) {
    payload["caption"] = caption;
  }

  if (disable_notification) {
    payload["disable_notification"] = disable_notification;
  }

  if (reply_to_message_id && reply_to_message_id != 0) {
    payload["reply_to_message_id"] = reply_to_message_id;
  }

  if (keyboard) {
    JsonObject& replyMarkup = payload.createNestedObject("reply_markup");

    DynamicJsonBuffer keyboardBuffer;
    replyMarkup["keyboard"] = keyboardBuffer.parseArray(keyboard);
  }

  return sendPostPhoto(payload);
}

bool UniversalTelegramBot::checkForOkResponse(String response) {
  int responseLength = response.length();

  for (int m=5; m < responseLength+1; m++)  {
    if (response.substring(m-10,m)=="{\"ok\":true") { //Chek if message has been properly sent
      return true;
    }
  }

  return false;
}

bool UniversalTelegramBot::sendChatAction(String chat_id, String text)  {

  bool sent = false;
  
  #ifdef DEBUG_U_TelegramBot
  if (_debug) Serial.println(F("SEND Chat Action Message"));
  #endif
  
  long sttime = millis() + RESPONSE_TIMEOUT;

  if (text != "") {
    while (millis() < sttime) {    // loop for a while to send the message
      String command="bot"+_token+"/sendChatAction?chat_id="+chat_id+"&action="+text;
      String response = sendGetToTelegram(command);
	  #ifdef DEBUG_U_TelegramBot
      if (_debug) Serial.println(response);
	  #endif
      sent = checkForOkResponse(response);

      if (sent) {
        break;
      }
    }
  }

  return sent;
}
