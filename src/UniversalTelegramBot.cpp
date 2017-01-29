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

#include "UniversalTelegramBot.h"

UniversalTelegramBot::UniversalTelegramBot(String token, Client &client)	{
  _token = token;
  this->client = &client;
}

String UniversalTelegramBot::sendGetToTelegram(String command) {
	String mess = "";
	long now;
	bool avail;

	// Connect with api.telegram.org
	if (client->connect(HOST, SSL_PORT)) {
		if (_debug) Serial.println(".... connected to server");
		String a="";
		char c;
		int ch_count=0;
		client->println("GET /"+command);
		now=millis();
		avail=false;
		while (millis() - now<1500) {
			while (client->available()) {
				char c = client->read();
				//Serial.write(c);
				if (ch_count < maxMessageLength)  {
					mess=mess+c;
					ch_count++;
				}
				avail=true;
			}
			if (avail) {
        if (_debug) {
          Serial.println();
          Serial.println(mess);
          Serial.println();
        }
				break;
			}
		}
	}

	return mess;
}

String UniversalTelegramBot::sendPostToTelegram(String command, JsonObject& payload){

  String body = "";
  String headers = "";
	long now;
	bool responseReceived;

	// Connect with api.telegram.org
	if (client->connect(HOST, SSL_PORT)) {
    // POST URI
    client->print("POST /" + command); client->println(" HTTP/1.1");
    // Host header
    client->print("Host:"); client->println(HOST);
    // JSON content type
    client->println("Content-Type: application/json");

    // Content length
    int length = payload.measureLength();
    client->print("Content-Length:"); client->println(length);
    // End of headers
    client->println();
    // POST message body
    //json.printTo(client); // very slow ??
    String out;
    payload.printTo(out);
    client->println(out);

    int ch_count=0;
    char c;
    now=millis();
		responseReceived=false;
    bool finishedHeaders = false;
    bool currentLineIsBlank = true;
		while (millis()-now<1500) {
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
        if (_debug) {
          Serial.println();
          Serial.println(body);
          Serial.println();
        }
  			break;
  		}
		}
  }

  return body;
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
  String boundry = "------------------------b8f610217e83e29b";
	// Connect with api.telegram.org
  if (client->connect(HOST, SSL_PORT)) {

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

    client->print("POST /bot"+_token+"/" + command); client->println(" HTTP/1.1");
    // Host header
    client->print("Host: "); client->println(HOST);
    client->println("User-Agent: arduino/1.0");
    client->println("Accept: */*");

    int contentLength = fileSize + start_request.length() + end_request.length();
    if (_debug) Serial.println("Content-Length: " + String(contentLength));
    client->print("Content-Length: "); client->println(String(contentLength));
    client->println("Content-Type: multipart/form-data; boundary=" + boundry);
    client->println("");

    client->print(start_request);

    if (_debug) Serial.print(start_request);

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
        if (_debug) {
          Serial.println("Sending full buffer");
        }
        client->write((const uint8_t *)buffer, 512);
        count = 0;
      }
    }

    if(count > 0) {
      if (_debug) {
        Serial.println("Sending remaining buffer");
      }
      client->write((const uint8_t *)buffer, count);
    }

    client->print(end_request);
    if (_debug) Serial.print(end_request);

    count = 0;
    int ch_count=0;
    char c;
    now=millis();
    bool finishedHeaders = false;
    bool currentLineIsBlank = true;
    while (millis()-now<1500) {
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
        if (_debug) {
          Serial.println();
          Serial.println(body);
          Serial.println();
        }
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

  if (_debug) Serial.println("GET Update Messages");

  String command = "bot"+_token+"/getUpdates?offset="+String(offset)+"&limit="+String(HANDLE_MESSAGES);
  String response = sendGetToTelegram(command); //receive reply from telegram.org

  if (response != "") {
    if (_debug)  {
      Serial.print("incoming message length");
      Serial.println(response.length());
      Serial.println("Creating DynamicJsonBuffer");
    }

    // Parse response into Json object
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);

    if (root.success()) {
      // root.printTo(Serial);
      if (_debug) Serial.println();
      if (root.containsKey("result")) {
        int resultArrayLength = root["result"].size();
        if (resultArrayLength > 0) {
          int newMessageIndex = 0;
          for (int i=0; i < resultArrayLength; i++) {
            JsonObject& message = root["result"][i]["message"];
            int update_id = root["result"][i]["update_id"];
            if (last_message_received != update_id) {
              last_message_received = update_id;
              String text = message["text"];
              String date = message["date"];
              String chat_id = message["chat"]["id"];
              String from_id = message["from"]["id"];
              String from_name = message["from"]["first_name"];

              messages[newMessageIndex].update_id = update_id;
              messages[newMessageIndex].text = text;
              messages[newMessageIndex].date = date;
              messages[newMessageIndex].chat_id = chat_id;
              messages[newMessageIndex].from_id = from_id;
              messages[newMessageIndex].from_name = from_name;

              newMessageIndex++;
            }
          }
          return newMessageIndex;
        } else {
          if (_debug) Serial.println("no new messages");
        }
      } else {
        if (_debug) Serial.println("Response contained no 'result'");
      }
    } else {
      // Buffer may not be big enough, increase buffer or reduce max number of messages
      if (_debug) Serial.println("Failed to parse update, the message could be too big for the buffer");
    }

    return 0;
  }
}

/***********************************************************************
* SendMessage - function to send message to telegram                  *
* (Arguments to pass: chat_id, text to transmit and markup(optional)) *
***********************************************************************/
bool UniversalTelegramBot::sendSimpleMessage(String chat_id, String text, String parse_mode)  {

  bool sent = false;
  if (_debug) Serial.println("SEND Simple Message");
  long sttime = millis();

  if (text!="") {
    while (millis() < sttime+8000) {    // loop for a while to send the message
      String command="bot"+_token+"/sendMessage?chat_id="+chat_id+"&text="+text+"&parse_mode="+parse_mode;
      String response = sendGetToTelegram(command);
      if (_debug) Serial.println(response);
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

  bool sent=false;
  if (_debug) Serial.println("SEND Post Message");
  long sttime=millis();

  if (payload.containsKey("text")) {
    while (millis() < sttime+8000) { // loop for a while to send the message
      String command = "bot"+_token+"/sendMessage";
      String response = sendPostToTelegram(command, payload);
      if (_debug) Serial.println(response);
      sent = checkForOkResponse(response);
      if (sent) {
        break;
      }
    }
  }

  return sent;
}

String UniversalTelegramBot::sendPostPhoto(JsonObject& payload)  {

  bool sent=false;
  String response = "";
  if (_debug) Serial.println("SEND Post Photo");
  long sttime=millis();

  if (payload.containsKey("photo")) {
    while (millis() < sttime+8000) { // loop for a while to send the message
      String command = "bot"+_token+"/sendPhoto";
      response = sendPostToTelegram(command, payload);
      if (_debug) Serial.println(response);
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

  if (_debug) Serial.println("SEND Photo");

  String response = sendMultipartFormDataToTelegram("sendPhoto", "photo", "img.jpg",
    contentType, chat_id, fileSize,
    moreDataAvailableCallback, getNextByteCallback);

  if (_debug) Serial.println(response);

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
  if (_debug) Serial.println("SEND Chat Action Message");
  long sttime = millis();

  if (text != "") {
    while (millis() < sttime+8000) {    // loop for a while to send the message
      String command="bot"+_token+"/sendChatAction?chat_id="+chat_id+"&action="+text;
      String response = sendGetToTelegram(command);

      if (_debug) Serial.println(response);
      sent = checkForOkResponse(response);

      if (sent) {
        break;
      }
    }
  }

  return sent;
}
