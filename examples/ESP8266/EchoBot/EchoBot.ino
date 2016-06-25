/*******************************************************************
 *  An example of bot that echos back any messages received        *
 *  using ESP8266TelegramBot.                                      *
 *                                                                 *
 *  written by Giacarlo Bacchio (Gianbacchio on Github)            *
 *  adapted by Brian Lough                                         *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>


// Initialize Wifi connection to the router
char ssid[] = "xxxxxxxxxxxxxxxxxxxxxxxx";              // your network SSID (name)
char password[] = "yyyyyyyyy";                              // your network key



// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get off Botfather)
ESP8266TelegramBOT bot(BOTtoken);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done


/********************************************
 * EchoMessages - function to Echo messages *
 ********************************************/
void Bot_EchoMessages(int numNewMessages) {
  for(int i=0; i<numNewMessages; i++) {
    bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].text, "");
  }
}


void setup() {

  Serial.begin(115200);
  delay(3000);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  bot.begin();      // launch Bot functionalities
}



void loop() {

  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_recived + 1);   // launch API GetUpdates up to xxx message
    if(numNewMessages) {
      Serial.println("got response");
      Bot_EchoMessages(numNewMessages);   // reply to message with Echo
    }
    Bot_lasttime = millis();
  }
}
