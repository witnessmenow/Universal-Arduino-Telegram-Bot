/*******************************************************************
 *  An example of how to use a custom reply keyboard.              *
 *                                                                 *
 *                                                                 *
 *  written by Brian Lough                                         *
 *******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


// Initialize Wifi connection to the router
char ssid[] = "xxxxxxxxxxxxxxxxxxxxxx";              // your network SSID (name)
char pass[] = "yyyyyyyy";                              // your network key



const int ledPin = 13;

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get off Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
int ledStatus = 0;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for(int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    if (text == "/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }
    if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }
    if (text == "/status") {
      if(ledStatus){
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }
    if (text == "/options") {
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& payload = jsonBuffer.createObject();
      payload["chat_id"] = chat_id;
      payload["text"] = "Choose from one of the following options";
      JsonObject& replyMarkup = payload.createNestedObject("reply_markup");

      // Reply keyboard is an array of arrays.
      // Outer array represents rows
      // Inner arrays represents columns
      // This example "ledon" and "ledoff" are two buttons on the top row
      // and "status is a single button on the next row"
      StaticJsonBuffer<200> keyboardBuffer;
      char keyboardJson[] = "[[\"/ledon\", \"/ledoff\"],[\"/status\"]]";
      replyMarkup["keyboard"] = keyboardBuffer.parseArray(keyboardJson);
      replyMarkup["resize_keyboard"] = true;

      bot.sendPostMessage(payload);
    }

    if (text == "/start") {
      String wellcome = "The custom keyboard example for ESP8266TelegramBot";
      String wellcome1 = "/ledon : to switch the Led ON";
      String wellcome2 = "/ledoff : to switch the Led OFF";
      String wellcome3 = "/status : Returns current status of LED";
      String wellcome4 = "/options : returns the custom keyboard";
      bot.sendMessage(chat_id, wellcome, "");
      bot.sendMessage(chat_id, wellcome1, "");
      bot.sendMessage(chat_id, wellcome2, "");
      bot.sendMessage(chat_id, wellcome3, "");
      bot.sendMessage(chat_id, wellcome4, "");
    }
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

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); //initilase pin as off

}



void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {

    int numNewMessages = bot.getUpdates(bot.last_message_recived + 1);
    if(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
    }
    Bot_lasttime = millis();
  }
}
