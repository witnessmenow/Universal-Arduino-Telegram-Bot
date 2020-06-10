#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <UniversalTelegramBot.h>

#define BOTtoken "XXXXXXXXXX:YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" // Change it to your bot token

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

void bot_setup() {
  client.setInsecure(); // Required for ESP8266
  const String commands = F("["
    "{\"command\":\"help\",  \"description\":\"Get bot usage help\"},"
    "{\"command\":\"start\", \"description\":\"Message sent when you open a chat with a bot\"},"
    "{\"command\":\"status\",\"description\":\"Answer device current status\"}"  // no comma on last command
  "]");
  bot.setMyCommands(commands);
  bot.sendMessage("25235518", "Hola amigo!", "Markdown");
}

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting Wifi: ");
  WiFi.begin(); // Using stored credentials
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\r\nWiFi connected");

  ArduinoOTA.begin();   // You should set a password for OTA. Ideally using MD5 hashes
  bot_setup();
}

void loop() {
  ArduinoOTA.handle();  // Only program on the serial port the first time. Then on WiFi! :-)
  
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    String answer;
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    for (int i = 0; i < numNewMessages; i++) {
      telegramMessage &msg = bot.messages[i];
      Serial.println("Received " + msg.text);
      if      (msg.text == "/help") answer = "So you need _help_, uh? me too! use /start or /status";
      else if (msg.text == "/start") answer = "Welcome my new friend! You are the first *" + msg.from_name + "* I've ever met";
      else if (msg.text == "/status") answer = "All is good here, thanks for asking!";
      else answer = "Say what?";

      bot.sendMessage(msg.chat_id, answer, "Markdown");
    }

    Bot_lasttime = millis();
  }
}
