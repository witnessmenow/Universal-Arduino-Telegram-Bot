
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


#include "101TelegramBOT.h"

101TelegramBOT::101TelegramBOT(String token) :TelegramBotCore(token) {
}

String 101TelegramBOT::sendGetToTelegram(String command) {
	String mess="";
	long now;
	bool avail;
	// Connect with api.telegram.org
	if (client.connect(HOST, SSL_PORT)) {
		// Serial.println(".... connected to server");
		String a="";
		char c;
		int ch_count=0;
		client.println("GET /"+command);
		now=millis();
		avail=false;
		while (millis()-now<1500) {
			while (client.available()) {
				char c = client.read();
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

String 101TelegramBOT::sendPostToTelegram(String command, JsonObject& payload){

  String response = "";
	long now;
	bool responseRecieved;
	// Connect with api.telegram.org
	if (client.connect(HOST, SSL_PORT)) {
    // POST URI
    client.print("POST /" + command); client.println(" HTTP/1.1");
    // Host header
    client.print("Host:"); client.println(HOST);
    // JSON content type
    client.println("Content-Type: application/json");
    // Content length
    int length = payload.measureLength();
    client.print("Content-Length:"); client.println(length);
    // End of headers
    client.println();
    // POST message body
    //json.printTo(client); // very slow ??
    String out;
    payload.printTo(out);
    client.println(out);

    int ch_count=0;
    char c;
    now=millis();
		responseRecieved=false;
		while (millis()-now<1500) {
			while (client.available()) {
				char c = client.read();
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
