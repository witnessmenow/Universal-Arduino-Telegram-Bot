/*******************************************************************
 *  An example of bot that receives commands and turns on and off  *
 *  an led using using A101TelegramBot.                             *
 *                                                                 *
 *  written by Giacarlo Bacchio (Gianbacchio on Github)            *
 *  adapted by Brian Lough                                         *
 *******************************************************************/

#include <WiFiSSLClient.h>
#include <WiFi101.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

WiFiSSLClient client;
UniversalTelegramBot bot(BOT_TOKEN, client);
unsigned long bot_lasttime; // last time messages' scan has been done

const int ledPin = 13;
bool Start = false;
int ledStatus = 0;

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    if (text == "/ledon")
    {
      digitalWrite(ledPin, HIGH); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }
    if (text == "/ledoff")
    {
      ledStatus = 0;
      digitalWrite(ledPin, LOW); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }
    if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }
    if (text == "/start")
    {
      String from_name = bot.messages[i].from_name;
      if (from_name == "")
        from_name = "Anonymous";

      String welcome = "Welcome, " + from_name + ", from FlashLedBot, your personal Bot on Arduino 101\n";
      welcome = welcome + "/ledon : to switch the Led ON \n";
      welcome = welcome + "/ledoff : to switch the Led OFF \n";
      welcome = welcome + "/status : Returns current status of LED \n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); //initialize pin as off
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
