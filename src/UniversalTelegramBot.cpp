
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
	String mess="";
	long now;
	bool avail;
	// Connect with api.telegram.org
	if (client->connect(HOST, SSL_PORT)) {
		// Serial.println(".... connected to server");
		String a="";
		char c;
		int ch_count=0;
		client->println("GET /"+command);
		now=millis();
		avail=false;
		while (millis()-now<1500) {
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
				// Serial.println();
				// Serial.println(mess);
				// Serial.println();
				break;
			}
		}
	}
	return mess;
}

String UniversalTelegramBot::sendPostToTelegram(String command, JsonObject& payload){

  String response = "";
	long now;
	bool responseRecieved;
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
		responseRecieved=false;
		while (millis()-now<1500) {
			while (client->available()) {
				char c = client->read();
				//Serial.write(c);
				if (ch_count < maxMessageLength)  {
					response=response+c;
					ch_count++;
				}
				responseRecieved=true;
			}
			if (responseRecieved) {
				// Serial.println();
				// Serial.println(response);
				// Serial.println();
				break;
			}
		}
  }

  return response;
}


bool UniversalTelegramBot::getMe() {
  String command="bot"+_token+"/getMe";
  String response = sendGetToTelegram(command);       //recieve reply from telegram.org
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

// JsonObject * UniversalTelegramBot::parseUpdates(String response) {
//   StaticJsonBuffer<MAX_BUFFER_SIZE> jsonBufferStatic;
//   return *jsonBufferStatic.parseObject(response);
// }

int UniversalTelegramBot::getUpdates(long offset)  {

  //Serial.println("GET Update Messages ");
  String command="bot"+_token+"/getUpdates?offset="+String(offset)+"&limit="+String(HANDLE_MESSAGES);
  String response = sendGetToTelegram(command);       //recieve reply from telegram.org
  if (response != "") {
    // Serial.print("incoming message length");
    // Serial.println(response.length());
    // Serial.print("Creating StaticJsonBuffer of size: ");
    // Serial.println(MAX_BUFFER_SIZE);

    // Parse response into Json object
    //StaticJsonBuffer<MAX_BUFFER_SIZE> jsonBuffer;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response);

    if(root.success()) {
      // root.printTo(Serial);
      // Serial.println();
      if (root.containsKey("result")) {
        int resultArrayLength = root["result"].size();
        if(resultArrayLength > 0) {
          int newMessageIndex = 0;
          for(int i=0; i < resultArrayLength; i++){
            int update_id = root["result"][i]["update_id"];
            if(last_message_recived != update_id) {
              last_message_recived = update_id;
              String text = root["result"][i]["message"]["text"];
              String date = root["result"][i]["message"]["date"];
              String chat_id = root["result"][i]["message"]["chat"]["id"];
              String from_id = root["result"][i]["message"]["from"]["id"];

              messages[newMessageIndex].update_id = update_id;
              messages[newMessageIndex].text = text;
              messages[newMessageIndex].date = date;
              messages[newMessageIndex].chat_id = chat_id;
              messages[newMessageIndex].from_id = from_id;

              newMessageIndex++;
            }
          }
          return newMessageIndex;
        } else {
          //Serial.println("no new messages");
        }
      } else {
        Serial.println("Response contained no 'result'");
      }
    } else {
      // Buffer may not be big enough, increase buffer or reduce max number of messages
      Serial.println("Failed to parse update, the message could be too big for the buffer");
    }

    return 0;
  }
}

/***********************************************************************
* SendMessage - function to send message to telegram                  *
* (Arguments to pass: chat_id, text to transmit and markup(optional)) *
***********************************************************************/
bool UniversalTelegramBot::sendSimpleMessage(String chat_id, String text, String parse_mode)  {

  bool sent=false;
  Serial.println("SEND Simple Message ");
  long sttime=millis();
  if (text!="") {
    while (millis()<sttime+8000) {    // loop for a while to send the message
      String command="bot"+_token+"/sendMessage?chat_id="+chat_id+"&text="+text+"&parse_mode="+parse_mode;
      String response = sendGetToTelegram(command);
      Serial.println(response);
      sent = checkForOkResponse(response);
      if(sent){
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
  if(parse_mode != ""){
    payload["parse_mode"] = parse_mode;
  }

  return sendPostMessage(payload);
}

bool UniversalTelegramBot::sendMessageWithReplyKeyboard(String chat_id, String text, String parse_mode, String keyboard, bool resize, bool oneTime, bool selective)  {


  DynamicJsonBuffer jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();
  payload["chat_id"] = chat_id;
  payload["text"] = text;
  if(parse_mode != ""){
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
  if(resize){
    replyMarkup["resize_keyboard"] = resize;
  }

  if(oneTime){
    replyMarkup["one_time_keyboard"] = oneTime;
  }

  if(selective){
    replyMarkup["selective"] = selective;
  }

  return sendPostMessage(payload);
}

/***********************************************************************
* SendPostMessage - function to send message to telegram                  *
* (Arguments to pass: chat_id, text to transmit and markup(optional)) *
***********************************************************************/
bool UniversalTelegramBot::sendPostMessage(JsonObject& payload)  {

  bool sent=false;
  Serial.println("SEND Post Message ");
  long sttime=millis();
  if (payload.containsKey("text")) {
    while (millis()<sttime+8000) {    // loop for a while to send the message
      String command = "bot"+_token+"/sendMessage";
      String response = sendPostToTelegram(command, payload);
      Serial.println(response);
      sent = checkForOkResponse(response);
      if(sent){
        break;
      }
    }
  }

  return sent;
}

bool UniversalTelegramBot::checkForOkResponse(String response) {
  int responseLength = response.length();
  for (int m=5; m<responseLength+1; m++)  {
    if (response.substring(m-10,m)=="{\"ok\":true")     {  //Chek if message has been properly sent
      return true;
    }
  }

  return false;
}
