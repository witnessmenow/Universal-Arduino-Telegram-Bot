/*******************************************************************
 *  this is a basic example how to program a Telegram Bot          *
 *  using TelegramBOT library on ESP8266                           *
 *                                                                 *
 *  Open a conversation with the bot, it will echo your messages   *
 *  https://web.telegram.org/#/im?p=@EchoBot_bot                   *                                                                 
 *                                                                 *
 *  written by Giacarlo Bacchio                                    *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>


// Initialize Wifi connection to the router
char ssid[] = "xxxxxxxxxxxxxxxxxxxxxxxx";              // your network SSID (name)
char password[] = "yyyyyyyyy";                              // your network key



// Initialize Telegram BOT
#define BOTtoken "77330665:AAEIHv4RJxPnygoKD8nZqLnlpmd4hq7iR7s"  //token of TestBOT
#define BOTname "EchoBot"
#define BOTusername "EchoBot_bot"
TelegramBOT bot(BOTtoken, BOTname, BOTusername);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done


/********************************************
 * EchoMessages - function to Echo messages *
 ********************************************/
void Bot_EchoMessages() {

  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
    bot.sendMessage(bot.message[i][4], bot.message[i][5], "");
  }
  bot.message[0][0] = "";   // All messages have been replied - reset new messages
}


void setup() {

  Serial.begin(115200);
  delay(3000);
  
  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
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
    bot.getUpdates(bot.message[0][1]);   // launch API GetUpdates up to xxx message
    Bot_EchoMessages();   // reply to message with Echo
    Bot_lasttime = millis();
  }
}




