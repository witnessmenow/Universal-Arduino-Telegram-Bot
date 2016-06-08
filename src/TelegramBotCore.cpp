
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


#include "TelegramBotCore.h"

TelegramBotCore::TelegramBotCore(String token)	{
  _token=token;
}

void TelegramBotCore::begin(void)	{

}

bool TelegramBotCore::getMe() {
  String command="bot"+_token+"/getMe";
  String response = sendGetToTelegram(command);       //recieve reply from telegram.org
  StaticJsonBuffer<500> jsonBuffer;
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
int TelegramBotCore::getUpdates(int offset)  {

  //Serial.println("GET Update Messages ");
  String command="bot"+_token+"/getUpdates?offset="+String(offset)+"&limit="+String(HANDLE_MESSAGES);
  String response = sendGetToTelegram(command);       //recieve reply from telegram.org
  if (response != "") {
    // Serial.print("incoming message length");
    // Serial.println(response.length());
    // Serial.print("Creating StaticJsonBuffer of size: ");
    // Serial.println(MAX_BUFFER_SIZE);
    StaticJsonBuffer<MAX_BUFFER_SIZE> jsonBuffer;

    // Parse response into Json object
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

              messages[newMessageIndex].update_id = update_id;
              messages[newMessageIndex].text = text;
              messages[newMessageIndex].date = date;
              messages[newMessageIndex].chat_id = chat_id;

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
      Serial.println("Failed to parse update");
    }

    return 0;
  }
}

/***********************************************************************
* SendMessage - function to send message to telegram                  *
* (Arguments to pass: chat_id, text to transmit and markup(optional)) *
***********************************************************************/
void TelegramBotCore::sendMessage(String chat_id, String text, String reply_markup)  {

  bool sent=false;
  // Serial.println("SEND Message ");
  long sttime=millis();
  if (text!="") {
    while (millis()<sttime+8000) {    // loop for a while to send the message
      String command="bot"+_token+"/sendMessage?chat_id="+chat_id+"&text="+text+"&reply_markup="+reply_markup;
      String mess=sendGetToTelegram(command);
      //Serial.println(mess);
      int messageLenght=mess.length();
      for (int m=5; m<messageLenght+1; m++)  {
        if (mess.substring(m-10,m)=="{\"ok\":true")     {  //Chek if message has been properly sent
          sent=true;
          break;
        }
      }
      if (sent==true)   {
        //  Serial.print("Message delivred: \"");
        //  Serial.print(text);
        //  Serial.println("\"");
        //  Serial.println();
        break;
      }
      delay(1000);
      //	Serial.println("Retry");

    }
  }
  // if (sent==false) Serial.println("Message not delivered");
}
